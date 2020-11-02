/* =================================================================
 * Copyright (c) 2020 Botson Corp
 *
 * This program is proprietary and confidential information of Botson.
 * And may not be used unless duly authorized.
 *
 * Revision:
 * Date: 2020-07-08
 * Author: luc
 * Descriptions:
 *
 */
// =================================================================
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <SDK/program.hpp>
#include <SDK/shader.hpp>

#define WIDTH 480
#define HEIGHT 320

Program *program;
GLuint source_tex_id;
GLuint output_tex_id;
GLuint fbo_id;

void prepare_source_texture() {
    /** IEEE 754 2.0, NaN, -2.0, NaN */
    static const GLuint rgba32ui[4] = {0x3fffffff, 0xffffffff, 0xffffffff,
                                       0x12345678};
    static const GLushort rgba16ui[4] = {0x3fff, 0xffff, 0xffff, 0xffff};
    static const GLubyte rgba8ui[4] = {0x3f, 0xff, 0x00, 0x00};

    glGenTextures(1, &source_tex_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, source_tex_id);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32UI, 1, 1, 1, 0,
                 GL_RGBA_INTEGER, GL_UNSIGNED_INT, 0);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 1, 1, 1, GL_RGBA_INTEGER,
                    GL_UNSIGNED_INT, (GLubyte *)rgba32ui);

    // glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16UI, 1, 1, 1, 0,
    // GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, 0);
    // glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 1, 1, 1,
    // GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, (GLubyte *)rgba16ui);

    // glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8UI, 1, 1, 1, 0,
    // GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, 0);
    // glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 1, 1, 1,
    // GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, (GLubyte *)rgba8ui);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void prepare_output_texture() {
    glGenTextures(1, &output_tex_id);
    glBindTexture(GL_TEXTURE_2D, output_tex_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, WIDTH, HEIGHT, 0, GL_RED_INTEGER,
                 GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void init() {
    GLenum texture_swizzle[4] = {GL_RED, GL_GREEN, GL_GREEN, GL_GREEN};

    glViewport(0, 0, WIDTH, HEIGHT);

    prepare_source_texture();

    //    glBindTexture(GL_TEXTURE_2D, output_tex_id);
    //    glGenFramebuffers(1, &fbo_id);
    //    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    //    GL_TEXTURE_2D, output_tex_id, 0 /* level */);
    //    glBindTexture(GL_TEXTURE_2D, 0);

    std::string vs_path, fs_path;

#if defined(CONFIG_SHADER_PATH)
    const char *path = CONFIG_SHADER_PATH;
#else
    char *path = getenv("CONFIG_SHADER_PATH");
#endif
    if (!path) {
        std::cout << "Shaders Not Found.\n";
        exit(1);
    } else {
        std::string base_path = std::string(path);
        vs_path = base_path + "/render2tex.vert";
        fs_path = base_path + "/render2tex.frag";
    }

    program = Program::from(Shader::file(GL_VERTEX_SHADER, vs_path),
                            Shader::file(GL_FRAGMENT_SHADER, fs_path));
    program->activate();

    program->uniform("sampler", 0);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D_ARRAY, source_tex_id);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_R,
                    texture_swizzle[0]);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_G,
                    texture_swizzle[1]);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_B,
                    texture_swizzle[3]);
}

void display_callback() {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glutSwapBuffers();
}

void idle_callback() {}

void reshape_callback(int width, int height) {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
    glViewport(0, 0, width, height);
}

void close_callback() {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_R, GL_RED);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void keyboard_callback(unsigned char key, int x, int y) {}

void keyboard_up_callback(unsigned char key, int x, int y) {}

void mouse_callback(int button, int state, int x, int y) {}

void entry_callback(int state) {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
}

void visibility_callback(int state) {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
}

int main(int argc, char **argv) {
    int win1, win2;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WIDTH, HEIGHT);

    win1 = glutCreateWindow("test");
    glutIdleFunc(idle_callback);
    glutDisplayFunc(display_callback);
    glutReshapeFunc(reshape_callback);
    glutKeyboardFunc(keyboard_callback);
    glutKeyboardUpFunc(keyboard_up_callback);
    glutMouseFunc(mouse_callback);
    glutEntryFunc(entry_callback);
    glutVisibilityFunc(visibility_callback);
    init();
    glutMainLoop();

    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
    return 0;
}
