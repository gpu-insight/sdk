/* =================================================================
 * Copyright (c) 2020 Botson Corp
 *
 * This program is proprietary and confidential information of Botson.
 * And may not be used unless duly authorized.
 *
 * Revision:
 * Date: 2020-09-08
 * Author: Luc
 * Descriptions:
 *  This is a simplest demo that renders to texture by compute shader.
 */
// =================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <sis/program.hpp>
#include <sis/shader.hpp>
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

/** Event handler results: */
#define NOP 0
#define EXIT 1
#define DRAW 2

static GLboolean fullscreen = GL_FALSE;
static GLboolean bmp = GL_FALSE;
static GLint samples = 0;
static GLboolean animate = GL_TRUE;

unsigned int winWidth = 300, winHeight = 300;
unsigned int texWidth = 64, texHeight = 64;

std::string data_path = std::string(SAMPLES_DATA_PATH);
Program *render_prog;
Program *compute_prog;

static void gen_tex() {
    GLuint tex;

    glGenTextures(1, &tex);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texHeight, 0, GL_RED, GL_FLOAT, NULL);

    glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
}

static void gen_cs_prog() {
    compute_prog = Program::from(
        Shader::file(GL_COMPUTE_SHADER, data_path + "/shaders/render2tex.comp"));

    compute_prog->activate();
    compute_prog->uniform("destTex", 0);
}

static void gen_render_prog() {
    render_prog = Program::from(
        Shader::file(GL_VERTEX_SHADER, data_path + "/shaders/render2tex.vert"),
        Shader::file(GL_FRAGMENT_SHADER, data_path + "/shaders/render2tex.frag"));

    render_prog->activate();
    render_prog->uniform("srcTex", 0);
}

static void update_tex(int frame) {
    compute_prog->activate();

    compute_prog->uniform("roll", frame * 0.01f);
    glDispatchCompute(texWidth/8, texHeight/8, 1);
}

static void render(void) {
    render_prog->activate();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/** Draw single frame, do SwapBuffers, compute FPS */
static void draw_frame(Display *dpy, Window win) {
    static int frames = 0;
    static double tRate0 = -1.0;
    double t = current_time();

    if (animate) {
        /* update texture for next frame */
        update_tex(frames);
    }

    render();
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
    /* Set the viewport */
    glViewport(0, 0, (GLint)width, (GLint)height);
}

static void init(void) {
    /* Generate shader programs */
    gen_tex();
    gen_render_prog();
    gen_cs_prog();
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
    int i = 0;

    int scrnum;
    XSetWindowAttributes attr;
    unsigned long mask;
    Window root;
    Window win;
    GLXContext ctx;
    XVisualInfo *visinfo;

    /* Singleton attributes. */
    attribs[i++] = GLX_RGBA;
    attribs[i++] = GLX_DOUBLEBUFFER;

    /* Key/value attributes. */
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

    visinfo = glXChooseVisual(dpy, scrnum, attribs);
    if (!visinfo) {
        printf("Error: couldn't get an RGB, Double-buffered");
        if (samples > 0)
            printf(", Multisample");
        printf(" visual\n");
        exit(1);
    }

    /* window attributes */
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
    /* XXX this is a bad way to get a borderless window! */
    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

    win = XCreateWindow(dpy, root, x, y, width, height, 0, visinfo->depth,
                        InputOutput, visinfo->visual, mask, &attr);

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

    ctx = glXCreateContext(dpy, visinfo, NULL, True);
    if (!ctx) {
        printf("Error: glXCreateContext failed\n");
        exit(1);
    }

    *winRet = win;
    *ctxRet = ctx;
    *visRet = visinfo->visualid;

    XFree(visinfo);
}

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
        XLookupString(&event->xkey, buffer, sizeof(buffer), NULL, NULL);
        if (buffer[0] == 27) {
            /* escape */
            return EXIT;
        } else if (buffer[0] == 'a' || buffer[0] == 'A') {
            animate = !animate;
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

        if (bmp) {
            snprintf(filename, sizeof(filename), "ren2tex-%014llu.bmp", frame++);
            save2bmp(filename, winWidth, winHeight);
        }
    }
}

static void usage(void) {
    printf("Usage:\n");
    printf("  -display <displayname>  set the display to run on\n");
    printf("  -bmp                    save frames into bitmaps\n");
    printf("  -samples N              run in multisample mode with at least N "
           "samples\n");
    printf("  -fullscreen             run in fullscreen mode\n");
    printf("  -info                   display OpenGL renderer info\n");
    printf("  -geometry WxH+X+Y       window geometry\n");
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

    make_window(dpy, "ren2tex", x, y, winWidth, winHeight, &win, &ctx, &visId);
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

