/* =================================================================
 * Copyright (c) 2020 Botson Corp
 *
 * This program is proprietary and confidential information of Botson.
 * And may not be used unless duly authorized.
 *
 * Revision:
 * Date: 2020-09-04
 * Author: Luc
 * Descriptions:
 *  This is an advanced version of the famous "glxgears" that requires
 *  at least OpenGL 3.3 core profile.
 */
// =================================================================

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <SDK/program.hpp>
#include <SDK/shader.hpp>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "config.h"
#include "save2bmp.h"

#ifndef GLX_MESA_swap_control
#define GLX_MESA_swap_control 1
typedef int (*PFNGLXGETSWAPINTERVALMESAPROC)(void);
#endif

#define BENCHMARK

#ifdef BENCHMARK

/* XXX this probably isn't very portable */

#include <sys/time.h>
#include <unistd.h>

/* return current time (in seconds) */
static double current_time(void) {
    struct timeval tv;
#ifdef __VMS
    (void)gettimeofday(&tv, NULL);
#else
    struct timezone tz;
    (void)gettimeofday(&tv, &tz);
#endif
    return (double)tv.tv_sec + tv.tv_usec / 1000000.0;
}

#else /*BENCHMARK*/

/* dummy */
static double current_time(void) {
    /* update this function for other platforms! */
    static double t = 0.0;
    static int warn = 1;
    if (warn) {
        fprintf(stderr, "Warning: current_time() not implemented!!\n");
        warn = 0;
    }
    return t += 1.0;
}

#endif /*BENCHMARK*/

#ifndef M_PI
#define M_PI 3.14159265
#endif

/** Event handler results: */
#define NOP 0
#define EXIT 1
#define DRAW 2

#define STRIPS_PER_TOOTH 7
#define VERTICES_PER_TOOTH 34
#define GEAR_VERTEX_STRIDE 6

/**
 * Struct describing the vertices in triangle strip
 */
struct vertex_strip {
    /** The first vertex in the strip */
    GLint first;
    /** The number of consecutive vertices in the strip after the first */
    GLint count;
};

/* Each vertex consist of GEAR_VERTEX_STRIDE GLfloat attributes */
typedef GLfloat GearVertex[GEAR_VERTEX_STRIDE];

/**
 * Struct representing a gear.
 */
struct gear {
    /** The array of vertices comprising the gear */
    GearVertex *vertices;
    /** The number of vertices comprising the gear */
    int nvertices;
    /** The array of triangle strips comprising the gear */
    struct vertex_strip *strips;
    /** The number of triangle strips comprising the gear */
    int nstrips;
    /** The Vertex Buffer Object holding the vertices in the graphics card */
    GLuint vbo;
    /** for Mesa's llvmpipe. if no, glDrawArrays(no VAO bound) */
    GLuint vao;
};

/** The view rotation */
static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
/** The gears */
static struct gear *gear1, *gear2, *gear3;
/** The current gear rotation angle */
static GLfloat angle = 0.0;
/** The linked shader program */
static Program *program;
/** The location of the shader uniforms */
static GLuint ModelViewProjectionMatrix_location, NormalMatrix_location,
    LightSourcePosition_location, MaterialColor_location;
/** The projection matrix */
static GLfloat ProjectionMatrix[16];
/** The direction of the directional light for the scene */
static const GLfloat LightSourcePosition[4] = {5.0, 5.0, 10.0, 1.0};

typedef enum {
    GEAR_RED = 1,
    GEAR_GREEN = 2,
    GEAR_BLUE = 4,
    GEAR_ALL = 7,
} gear_mask;

static gear_mask gear_filter = GEAR_ALL;
static GLboolean fullscreen = GL_FALSE; /* Create a single fullscreen window */
static GLboolean bmp = GL_FALSE;     /* Enable stereo.  */
static GLint samples = 0;           /* Choose visual with at least N samples. */
static GLboolean animate = GL_TRUE; /* Animation */

static unsigned int winWidth = 300, winHeight = 300;

/**
 * Determine whether or not a GLX extension is supported.
 */
static int is_glx_extension_supported(Display *dpy, const char *query) {
    const int scrnum = DefaultScreen(dpy);
    const char *glx_extensions = NULL;
    const size_t len = strlen(query);
    const char *ptr;

    if (glx_extensions == NULL) {
        glx_extensions = glXQueryExtensionsString(dpy, scrnum);
    }

    ptr = strstr(glx_extensions, query);
    return ((ptr != NULL) && ((ptr[len] == ' ') || (ptr[len] == '\0')));
}

/**
 * Fills a gear vertex.
 *
 * @param v the vertex to fill
 * @param x the x coordinate
 * @param y the y coordinate
 * @param z the z coortinate
 * @param n pointer to the normal table
 *
 * @return the operation error code
 */
static GearVertex *vert(GearVertex *v, GLfloat x, GLfloat y, GLfloat z,
                        GLfloat n[3]) {
    v[0][0] = x;
    v[0][1] = y;
    v[0][2] = z;
    v[0][3] = n[0];
    v[0][4] = n[1];
    v[0][5] = n[2];

    return v + 1;
}

/**
 *  Create a gear wheel.
 *
 *  @param inner_radius radius of hole at center
 *  @param outer_radius radius at center of teeth
 *  @param width width of gear
 *  @param teeth number of teeth
 *  @param tooth_depth depth of tooth
 *
 *  @return pointer to the constructed struct gear
 */
static struct gear *create_gear(GLfloat inner_radius, GLfloat outer_radius,
                                GLfloat width, GLint teeth,
                                GLfloat tooth_depth) {
    GLfloat r0, r1, r2;
    GLfloat da;
    GearVertex *v;
    struct gear *gear;
    double s[5], c[5];
    GLfloat normal[3];
    int cur_strip = 0;
    int i;

    /* Allocate memory for the gear */
    gear = (struct gear *)malloc(sizeof *gear);
    if (gear == NULL)
        return NULL;

    /* Calculate the radii used in the gear */
    r0 = inner_radius;
    r1 = outer_radius - tooth_depth / 2.0;
    r2 = outer_radius + tooth_depth / 2.0;

    da = 2.0 * M_PI / teeth / 4.0;

    /* Allocate memory for the triangle strip information */
    gear->nstrips = STRIPS_PER_TOOTH * teeth;
    gear->strips =
        (struct vertex_strip *)calloc(gear->nstrips, sizeof(*gear->strips));

    /* Allocate memory for the vertices */
    gear->vertices = (GearVertex *)calloc(VERTICES_PER_TOOTH * teeth,
                                          sizeof(*gear->vertices));
    v = gear->vertices;

    for (i = 0; i < teeth; i++) {
        /* Calculate needed sin/cos for varius angles */
        sincos(i * 2.0 * M_PI / teeth, &s[0], &c[0]);
        sincos(i * 2.0 * M_PI / teeth + da, &s[1], &c[1]);
        sincos(i * 2.0 * M_PI / teeth + da * 2, &s[2], &c[2]);
        sincos(i * 2.0 * M_PI / teeth + da * 3, &s[3], &c[3]);
        sincos(i * 2.0 * M_PI / teeth + da * 4, &s[4], &c[4]);

        /* A set of macros for making the creation of the gears easier */
#define GEAR_POINT(r, da)                                                      \
    { (r) * c[(da)], (r)*s[(da)] }
#define SET_NORMAL(x, y, z)                                                    \
    do {                                                                       \
        normal[0] = (x);                                                       \
        normal[1] = (y);                                                       \
        normal[2] = (z);                                                       \
    } while (0)

#define GEAR_VERT(v, point, sign)                                              \
    vert((v), p[(point)].x, p[(point)].y, (sign)*width * 0.5, normal)

#define START_STRIP                                                            \
    do {                                                                       \
        gear->strips[cur_strip].first = v - gear->vertices;                    \
    } while (0);

#define END_STRIP                                                              \
    do {                                                                       \
        int _tmp = (v - gear->vertices);                                       \
        gear->strips[cur_strip].count = _tmp - gear->strips[cur_strip].first;  \
        cur_strip++;                                                           \
    } while (0)

#define QUAD_WITH_NORMAL(p1, p2)                                               \
    do {                                                                       \
        SET_NORMAL((p[(p1)].y - p[(p2)].y), -(p[(p1)].x - p[(p2)].x), 0);      \
        v = GEAR_VERT(v, (p1), -1);                                            \
        v = GEAR_VERT(v, (p1), 1);                                             \
        v = GEAR_VERT(v, (p2), -1);                                            \
        v = GEAR_VERT(v, (p2), 1);                                             \
    } while (0)

        struct point {
            GLfloat x;
            GLfloat y;
        };

        /* Create the 7 points (only x,y coords) used to draw a tooth */
        struct point p[7] = {
            GEAR_POINT(r2, 1), // 0
            GEAR_POINT(r2, 2), // 1
            GEAR_POINT(r1, 0), // 2
            GEAR_POINT(r1, 3), // 3
            GEAR_POINT(r0, 0), // 4
            GEAR_POINT(r1, 4), // 5
            GEAR_POINT(r0, 4), // 6
        };

        /* Front face */
        START_STRIP;
        SET_NORMAL(0, 0, 1.0);
        v = GEAR_VERT(v, 0, +1);
        v = GEAR_VERT(v, 1, +1);
        v = GEAR_VERT(v, 2, +1);
        v = GEAR_VERT(v, 3, +1);
        v = GEAR_VERT(v, 4, +1);
        v = GEAR_VERT(v, 5, +1);
        v = GEAR_VERT(v, 6, +1);
        END_STRIP;

        /* Inner face */
        START_STRIP;
        QUAD_WITH_NORMAL(4, 6);
        END_STRIP;

        /* Back face */
        START_STRIP;
        SET_NORMAL(0, 0, -1.0);
        v = GEAR_VERT(v, 6, -1);
        v = GEAR_VERT(v, 5, -1);
        v = GEAR_VERT(v, 4, -1);
        v = GEAR_VERT(v, 3, -1);
        v = GEAR_VERT(v, 2, -1);
        v = GEAR_VERT(v, 1, -1);
        v = GEAR_VERT(v, 0, -1);
        END_STRIP;

        /* Outer face */
        START_STRIP;
        QUAD_WITH_NORMAL(0, 2);
        END_STRIP;

        START_STRIP;
        QUAD_WITH_NORMAL(1, 0);
        END_STRIP;

        START_STRIP;
        QUAD_WITH_NORMAL(3, 1);
        END_STRIP;

        START_STRIP;
        QUAD_WITH_NORMAL(5, 3);
        END_STRIP;
    }

    gear->nvertices = (v - gear->vertices);

    /* Store the vertices in a vertex buffer object (VBO) */
    glGenBuffers(1, &gear->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, gear->vbo);
    glBufferData(GL_ARRAY_BUFFER, gear->nvertices * sizeof(GearVertex),
                 gear->vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &gear->vao);
    glBindVertexArray(gear->vao);

    return gear;
}

/**
 * Multiplies two 4x4 matrices.
 *
 * The result is stored in matrix m.
 *
 * @param m the first matrix to multiply
 * @param n the second matrix to multiply
 */
static void multiply(GLfloat *m, const GLfloat *n) {
    GLfloat tmp[16];
    const GLfloat *row, *column;
    div_t d;
    int i, j;

    for (i = 0; i < 16; i++) {
        tmp[i] = 0;
        d = div(i, 4);
        row = n + d.quot * 4;
        column = m + d.rem;
        for (j = 0; j < 4; j++)
            tmp[i] += row[j] * column[j * 4];
    }
    memcpy(m, &tmp, sizeof tmp);
}

/**
 * Rotates a 4x4 matrix.
 *
 * @param[in,out] m the matrix to rotate
 * @param angle the angle to rotate
 * @param x the x component of the direction to rotate to
 * @param y the y component of the direction to rotate to
 * @param z the z component of the direction to rotate to
 */
static void rotate(GLfloat *m, GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    double s, c;

    sincos(angle, &s, &c);
    GLfloat r[16] = {x * x * (1 - c) + c,
                     y * x * (1 - c) + z * s,
                     x * z * (1 - c) - y * s,
                     0,
                     x * y * (1 - c) - z * s,
                     y * y * (1 - c) + c,
                     y * z * (1 - c) + x * s,
                     0,
                     x * z * (1 - c) + y * s,
                     y * z * (1 - c) - x * s,
                     z * z * (1 - c) + c,
                     0,
                     0,
                     0,
                     0,
                     1};

    multiply(m, r);
}

/**
 * Translates a 4x4 matrix.
 *
 * @param[in,out] m the matrix to translate
 * @param x the x component of the direction to translate to
 * @param y the y component of the direction to translate to
 * @param z the z component of the direction to translate to
 */
static void translate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z) {
    GLfloat t[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1};

    multiply(m, t);
}

/**
 * Creates an identity 4x4 matrix.
 *
 * @param m the matrix make an identity matrix
 */
static void identity(GLfloat *m) {
    GLfloat t[16] = {
        1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0,
    };

    memcpy(m, t, sizeof(t));
}

/**
 * Transposes a 4x4 matrix.
 *
 * @param m the matrix to transpose
 */
static void transpose(GLfloat *m) {
    GLfloat t[16] = {m[0], m[4], m[8],  m[12], m[1], m[5], m[9],  m[13],
                     m[2], m[6], m[10], m[14], m[3], m[7], m[11], m[15]};

    memcpy(m, t, sizeof(t));
}

/**
 * Inverts a 4x4 matrix.
 *
 * This function can currently handle only pure translation-rotation matrices.
 * Read http://www.gamedev.net/community/forums/topic.asp?topic_id=425118
 * for an explanation.
 */
static void invert(GLfloat *m) {
    GLfloat t[16];
    identity(t);

    // Extract and invert the translation part 't'. The inverse of a
    // translation matrix can be calculated by negating the translation
    // coordinates.
    t[12] = -m[12];
    t[13] = -m[13];
    t[14] = -m[14];

    // Invert the rotation part 'r'. The inverse of a rotation matrix is
    // equal to its transpose.
    m[12] = m[13] = m[14] = 0;
    transpose(m);

    // inv(m) = inv(r) * inv(t)
    multiply(m, t);
}

/**
 * Calculate a perspective projection transformation.
 *
 * @param m the matrix to save the transformation in
 * @param fovy the field of view in the y direction
 * @param aspect the view aspect ratio
 * @param zNear the near clipping plane
 * @param zFar the far clipping plane
 */
void perspective(GLfloat *m, GLfloat fovy, GLfloat aspect, GLfloat zNear,
                 GLfloat zFar) {
    GLfloat tmp[16];
    identity(tmp);

    double sine, cosine, cotangent, deltaZ;
    GLfloat radians = fovy / 2 * M_PI / 180;

    deltaZ = zFar - zNear;
    sincos(radians, &sine, &cosine);

    if ((deltaZ == 0) || (sine == 0) || (aspect == 0))
        return;

    cotangent = cosine / sine;

    tmp[0] = cotangent / aspect;
    tmp[5] = cotangent;
    tmp[10] = -(zFar + zNear) / deltaZ;
    tmp[11] = -1;
    tmp[14] = -2 * zNear * zFar / deltaZ;
    tmp[15] = 0;

    memcpy(m, tmp, sizeof(tmp));
}


/**
 * Draws a gear.
 *
 * @param gear the gear to draw
 * @param transform the current transformation matrix
 * @param x the x position to draw the gear at
 * @param y the y position to draw the gear at
 * @param angle the rotation angle of the gear
 * @param color the color of the gear
 */
static void draw_gear(struct gear *gear, GLfloat *transform, GLfloat x,
                      GLfloat y, GLfloat angle, const GLfloat color[4]) {
    GLfloat model_view[16];
    GLfloat normal_matrix[16];
    GLfloat model_view_projection[16];

    /* Translate and rotate the gear */
    memcpy(model_view, transform, sizeof(model_view));
    translate(model_view, x, y, 0);
    rotate(model_view, 2 * M_PI * angle / 360.0, 0, 0, 1);

    /* Create and set the ModelViewProjectionMatrix */
    memcpy(model_view_projection, ProjectionMatrix,
           sizeof(model_view_projection));
    multiply(model_view_projection, model_view);

    glUniformMatrix4fv(ModelViewProjectionMatrix_location, 1, GL_FALSE,
                       model_view_projection);

    /*
     * Create and set the NormalMatrix. It's the inverse transpose of the
     * ModelView matrix.
     */
    memcpy(normal_matrix, model_view, sizeof(normal_matrix));
    invert(normal_matrix);
    transpose(normal_matrix);
    glUniformMatrix4fv(NormalMatrix_location, 1, GL_FALSE, normal_matrix);

    /* Set the gear color */
    glUniform4fv(MaterialColor_location, 1, color);

    /* Set the vertex buffer object to use */
    glBindBuffer(GL_ARRAY_BUFFER, gear->vbo);

    /* Set up the position of the attributes in the vertex buffer object */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), NULL);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (GLfloat *)0 + 3);

    /* Enable the attributes */
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    /* Draw the triangle strips that comprise the gear */
    int n;
    for (n = 0; n < gear->nstrips; n++)
        glDrawArrays(GL_TRIANGLE_STRIP, gear->strips[n].first,
                     gear->strips[n].count);

    /* Disable the attributes */
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

/**
 * Draws the gears.
 */
static void draw_gears(void) {
    const static GLfloat red[4] = {0.8, 0.1, 0.0, 1.0};
    const static GLfloat green[4] = {0.0, 0.8, 0.2, 1.0};
    const static GLfloat blue[4] = {0.2, 0.2, 1.0, 1.0};
    GLfloat transform[16];
    identity(transform);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Translate and rotate the view */
    translate(transform, 0, 0, -20);
    rotate(transform, 2 * M_PI * view_rotx / 360.0, 1, 0, 0);
    rotate(transform, 2 * M_PI * view_roty / 360.0, 0, 1, 0);
    rotate(transform, 2 * M_PI * view_rotz / 360.0, 0, 0, 1);

    /* Draw the gears */
    if (GEAR_RED & gear_filter) draw_gear(gear1, transform, -3.0, -2.0, angle, red);
    if (GEAR_GREEN & gear_filter) draw_gear(gear2, transform, 3.1, -2.0, -2 * angle - 9.0, green);
    if (GEAR_BLUE & gear_filter) draw_gear(gear3, transform, -3.1, 4.2, -2 * angle - 25.0, blue);
}

/** Draw single frame, do SwapBuffers, compute FPS */
static void draw_frame(Display *dpy, Window win) {
    static int frames = 0;
    static double tRot0 = -1.0, tRate0 = -1.0;
    double dt, t = current_time();

    if (tRot0 < 0.0)
        tRot0 = t;
    dt = t - tRot0;
    tRot0 = t;

    if (animate) {
        /* advance rotation for next frame */
        angle += 70.0 * dt; /* 70 degrees per second */
        if (angle > 3600.0)
            angle -= 3600.0;
    }

    draw_gears();
    glXSwapBuffers(dpy, win);

    frames++;

    if (tRate0 < 0.0)
        tRate0 = t;
    if (t - tRate0 >= 5.0) {
        GLfloat seconds = t - tRate0;
        GLfloat fps = frames / seconds;
        printf("%d frames in %3.1f seconds = %6.3f FPS\n", frames, seconds,
               fps);
        fflush(stdout);
        tRate0 = t;
        frames = 0;
    }
}

/* new window size or exposure */
static void reshape(int width, int height) {
    /* Update the projection matrix */
    perspective(ProjectionMatrix, 60.0, width / (float)height, 1.0, 1024.0);

    /* Set the viewport */
    glViewport(0, 0, (GLint)width, (GLint)height);
}

static void init(void) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    std::cout << glGetString(GL_VERSION) << "\n";
    /* Compile the vertex shader */
    std::string data_path = std::string(SAMPLES_DATA_PATH);

    program = Program::from(
        Shader::file(GL_VERTEX_SHADER, data_path + "/shaders/gear.vert"),
        Shader::file(GL_FRAGMENT_SHADER, data_path + "/shaders/gear.frag"));

    program->activate();

    /* Get the locations of the uniforms so we can access them */
    ModelViewProjectionMatrix_location =
        program->uniform("ModelViewProjectionMatrix");
    NormalMatrix_location =
        program->uniform("NormalMatrix");
    LightSourcePosition_location =
        program->uniform("LightSourcePosition");
    MaterialColor_location =
        program->uniform("MaterialColor");

    /* Set the LightSourcePosition uniform which is constant throught the
     * program */
    glUniform4fv(LightSourcePosition_location, 1, LightSourcePosition);

    /* make the gears */
    gear1 = create_gear(1.0, 4.0, 1.0, 20, 0.7);
    gear2 = create_gear(0.5, 2.0, 2.0, 10, 0.7);
    gear3 = create_gear(1.3, 2.0, 0.5, 10, 0.7);
}

/**
 * Remove window border/decorations.
 */
static void no_border(Display *dpy, Window w) {
    static const unsigned MWM_HINTS_DECORATIONS = (1 << 1);
    static const int PROP_MOTIF_WM_HINTS_ELEMENTS = 5;

    typedef struct {
        unsigned long flags;
        unsigned long functions;
        unsigned long decorations;
        long inputMode;
        unsigned long status;
    } PropMotifWmHints;

    PropMotifWmHints motif_hints;
    Atom prop, proptype;
    unsigned long flags = 0;

    /* setup the property */
    motif_hints.flags = MWM_HINTS_DECORATIONS;
    motif_hints.decorations = flags;

    /* get the atom for the property */
    prop = XInternAtom(dpy, "_MOTIF_WM_HINTS", True);
    if (!prop) {
        /* something went wrong! */
        return;
    }

    /* not sure this is correct, seems to work, XA_WM_HINTS didn't work */
    proptype = prop;

    XChangeProperty(dpy, w,                        /* display, window */
                    prop, proptype,                /* property, type */
                    32,                            /* format: 32-bit datums */
                    PropModeReplace,               /* mode */
                    (unsigned char *)&motif_hints, /* data */
                    PROP_MOTIF_WM_HINTS_ELEMENTS   /* nelements */
    );
}

/*
 * Create an RGB, double-buffered window.
 * Return the window and context handles.
 */
static void make_window(Display *dpy, const char *name, int x, int y, int width,
                        int height, Window *winRet, GLXContext *ctxRet,
                        VisualID *visRet) {
    int attribs[64];
    int profile_attrib[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
	    GLX_CONTEXT_MINOR_VERSION_ARB, 3,
	    None
    };
    int i = 0;

    int scrnum;
    XSetWindowAttributes attr;
    unsigned long mask;
    Window root;
    Window win;
    XVisualInfo *vinfo;
    GLXContext ctx;
    GLXFBConfig *fbconfigs;
    int n;

    /* Key/value attributes. */
    attribs[i++] = GLX_RENDER_TYPE;
    attribs[i++] = GLX_RGBA_BIT;
    attribs[i++] = GLX_DOUBLEBUFFER;
    attribs[i++] = True;
    attribs[i++] = GLX_RED_SIZE;
    attribs[i++] = 1;
    attribs[i++] = GLX_GREEN_SIZE;
    attribs[i++] = 1;
    attribs[i++] = GLX_BLUE_SIZE;
    attribs[i++] = 1;
    attribs[i++] = GLX_DEPTH_SIZE;
    attribs[i++] = 1;
    if (samples > 0) {
        attribs[i++] = GLX_SAMPLE_BUFFERS;
        attribs[i++] = 1;
        attribs[i++] = GLX_SAMPLES;
        attribs[i++] = samples;
    }

    attribs[i++] = None;

    scrnum = DefaultScreen(dpy);
    root = RootWindow(dpy, scrnum);

    fbconfigs = glXChooseFBConfig(dpy, scrnum, attribs, &n);
    if (!fbconfigs) {
        printf("Error: couldn't get an RGB, Double-buffered");
        if (samples > 0)
            printf(", Multisample");
        printf(" FBConfig\n");
        exit(1);
    }

    vinfo = glXGetVisualFromFBConfig(dpy, fbconfigs[0]);
    /* window attributes */
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(dpy, root, vinfo->visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
    /* XXX this is a bad way to get a borderless window! */
    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

    win = XCreateWindow(dpy, root, x, y, width, height, 0, vinfo->depth,
                        InputOutput, vinfo->visual, mask, &attr);

    if (fullscreen)
        no_border(dpy, win);

    /* set hints and properties */
    {
        XSizeHints sizehints;
        sizehints.x = x;
        sizehints.y = y;
        sizehints.width = width;
        sizehints.height = height;
        sizehints.flags = USSize | USPosition;
        XSetNormalHints(dpy, win, &sizehints);
        XSetStandardProperties(dpy, win, name, name, None, (char **)NULL, 0,
                               &sizehints);
    }

    if (is_glx_extension_supported(dpy, "GLX_ARB_create_context_profile")) {
        PFNGLXCREATECONTEXTATTRIBSARBPROC pglXCreateContextAttribsARB =
                (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddressARB(
                        (const GLubyte *) "glXCreateContextAttribsARB");
        ctx = (*pglXCreateContextAttribsARB)(dpy, fbconfigs[0], NULL, True, profile_attrib);
    } else {
        ctx = glXCreateContext(dpy, vinfo, NULL, True);
    }

    if (!ctx) {
        printf("Error: failed to create GL context.\n");
        exit(1);
    }

    *winRet = win;
    *ctxRet = ctx;
    *visRet = vinfo->visualid;

    XFree(vinfo);
}

/**
 * Attempt to determine whether or not the display is synched to vblank.
 */
static void query_vsync(Display *dpy, GLXDrawable drawable) {
    int interval = 0;

#if defined(GLX_EXT_swap_control)
    if (is_glx_extension_supported(dpy, "GLX_EXT_swap_control")) {
        unsigned int tmp = -1;
        glXQueryDrawable(dpy, drawable, GLX_SWAP_INTERVAL_EXT, &tmp);
        interval = tmp;
    } else
#endif
        if (is_glx_extension_supported(dpy, "GLX_MESA_swap_control")) {
        PFNGLXGETSWAPINTERVALMESAPROC pglXGetSwapIntervalMESA =
            (PFNGLXGETSWAPINTERVALMESAPROC)glXGetProcAddressARB(
                (const GLubyte *)"glXGetSwapIntervalMESA");

        interval = (*pglXGetSwapIntervalMESA)();
    } else if (is_glx_extension_supported(dpy, "GLX_SGI_swap_control")) {
        /* The default swap interval with this extension is 1.  Assume that it
         * is set to the default.
         *
         * Many Mesa-based drivers default to 0, but all of these drivers also
         * export GLX_MESA_swap_control.  In that case, this branch will never
         * be taken, and the correct result should be reported.
         */
        interval = 1;
    }

    if (interval > 0) {
        printf("Running synchronized to the vertical refresh.  The framerate "
               "should be\n");
        if (interval == 1) {
            printf("approximately the same as the monitor refresh rate.\n");
        } else if (interval > 1) {
            printf("approximately 1/%d the monitor refresh rate.\n", interval);
        }
    }
}

/**
 * Handle one X event.
 * \return NOP, EXIT or DRAW
 */
static int handle_event(Display *dpy, Window win, XEvent *event) {
    (void)dpy;
    (void)win;

    switch (event->type) {
    case Expose:
        return DRAW;
    case ConfigureNotify:
        reshape(event->xconfigure.width, event->xconfigure.height);
        break;
    case KeyPress: {
        char buffer[10];
        int code;
        code = XLookupKeysym(&event->xkey, 0);
        if (code == XK_Left) {
            view_roty += 5.0;
        } else if (code == XK_Right) {
            view_roty -= 5.0;
        } else if (code == XK_Up) {
            view_rotx += 5.0;
        } else if (code == XK_Down) {
            view_rotx -= 5.0;
        } else {
            XLookupString(&event->xkey, buffer, sizeof(buffer), NULL, NULL);
            if (buffer[0] == 27) {
                /* escape */
                return EXIT;
            } else if (buffer[0] == 'a' || buffer[0] == 'A') {
                animate = !animate;
            }
        }
        return DRAW;
    }
    }
    return NOP;
}

static void event_loop(Display *dpy, Window win) {
    char filename[128] = {0};
    unsigned long long frame = 0;
    while (1) {
        int op;
        while (!animate || XPending(dpy) > 0) {
            XEvent event;
            XNextEvent(dpy, &event);
            op = handle_event(dpy, win, &event);
            if (op == EXIT)
                return;
            else if (op == DRAW)
                break;
        }

        draw_frame(dpy, win);

        if (bmp)
        {
            snprintf(filename, sizeof(filename), "glxgears-%020llu.bmp", frame++);
            save2bmp(filename, winWidth, winHeight);
        }
    }
}

static void usage(void) {
    printf("Usage:\n");
    printf("  -display <displayname>  set the display to run on\n");
    printf("  -bmp                    save in bitmap format\n");
    printf("  -samples N              run in multisample mode with at least N "
           "samples\n");
    printf("  -fullscreen             run in fullscreen mode\n");
    printf("  -info                   display OpenGL renderer info\n");
    printf("  -geometry WxH+X+Y       window geometry\n");
    printf("  -gears <mask>            which gears will be drawn\n");
}

int main(int argc, char *argv[]) {
    int x = 0, y = 0;
    Display *dpy;
    Window win;
    GLXContext ctx;
    char *dpyName = NULL;
    GLboolean printInfo = GL_FALSE;
    VisualID visId;
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-display") == 0) {
            dpyName = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-info") == 0) {
            printInfo = GL_TRUE;
        } else if (strcmp(argv[i], "-bmp") == 0) {
            bmp = GL_TRUE;
        } else if (i < argc - 1 && strcmp(argv[i], "-samples") == 0) {
            samples = strtod(argv[i + 1], NULL);
            ++i;
        } else if (i < argc - 1 && strcmp(argv[i], "-gears") == 0) {
            gear_filter = (gear_mask)strtod(argv[i + 1], NULL);
            ++i;
        } else if (strcmp(argv[i], "-fullscreen") == 0) {
            fullscreen = GL_TRUE;
        } else if (i < argc - 1 && strcmp(argv[i], "-geometry") == 0) {
            XParseGeometry(argv[i + 1], &x, &y, &winWidth, &winHeight);
            i++;
        } else {
            usage();
            return -1;
        }
    }

    dpy = XOpenDisplay(dpyName);
    if (!dpy) {
        printf("Error: couldn't open display %s\n",
               dpyName ? dpyName : getenv("DISPLAY"));
        return -1;
    }

    if (fullscreen) {
        int scrnum = DefaultScreen(dpy);

        x = 0;
        y = 0;
        winWidth = DisplayWidth(dpy, scrnum);
        winHeight = DisplayHeight(dpy, scrnum);
    }

    make_window(dpy, "glxgears", x, y, winWidth, winHeight, &win, &ctx, &visId);
    XMapWindow(dpy, win);
    glXMakeCurrent(dpy, win, ctx);
    query_vsync(dpy, win);

    if (printInfo) {
        printf("GL_RENDERER   = %s\n", (char *)glGetString(GL_RENDERER));
        printf("GL_VERSION    = %s\n", (char *)glGetString(GL_VERSION));
        printf("GL_VENDOR     = %s\n", (char *)glGetString(GL_VENDOR));
        printf("GL_EXTENSIONS = %s\n", (char *)glGetString(GL_EXTENSIONS));
        printf("VisualID %d, 0x%x\n", (int)visId, (int)visId);
    }

    init();

    /* Set initial projection/viewing transformation.
     * We can't be sure we'll get a ConfigureNotify event when the window
     * first appears.
     */
    reshape(winWidth, winHeight);

    event_loop(dpy, win);

    glXMakeCurrent(dpy, None, NULL);
    glXDestroyContext(dpy, ctx);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);

    return 0;
}

