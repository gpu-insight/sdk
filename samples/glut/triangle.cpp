/* =================================================================
 * Copyright (c) 2020 Botson Corp
 *
 * This program is proprietary and confidential information of Botson.
 * And may not be used unless duly authorized.
 *
 * Revision:
 * Date: 2020-04-16
 * Author:
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

GLuint VAOs, Buffers;
Program *program;
GLfloat red = 1.0;
GLfloat green = 1.0;
GLfloat blue = 1.0;

GLfloat x_off = 0.0;
GLfloat y_off = 0.0;
GLfloat z_off = 0.0;

GLuint start = 0;

void init() {
    GLint r_location;
    GLint g_location;
    GLint b_location;

    GLint x_location;
    GLint y_location;
    GLint z_location;

    GLfloat vertices[] = {
        -0.90f, -0.90f, 0.85f, -0.90f, -0.90f, 0.85f,
        0.90f,  -0.85f, 0.90f, 0.90f,  -0.85f, 0.90f,
    };

    glGenVertexArrays(1, &VAOs);
    glBindVertexArray(VAOs);

    glGenBuffers(1, &Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glClearColor(1.0, 1.0, 1.0, 0.0);

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
        vs_path = base_path + "/triangles.vert";
        fs_path = base_path + "/triangles.frag";
    }

    program = Program::from(Shader::file(GL_VERTEX_SHADER, vs_path),
                            Shader::file(GL_FRAGMENT_SHADER, fs_path));
    program->activate();

    program->uniform("red", 1.0f);
    program->uniform("green", 0.0f);
    program->uniform("blue", 1.0f);
    program->uniform("x_offset", x_off);
    program->uniform("y_offset", y_off);
    program->uniform("z_offset", z_off);
}

void display_callback() {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAOs);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glutSwapBuffers();
}

void idle_callback() {}

void reshape_callback(int width, int height) {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
    glViewport(0, 0, width, height);
}

void close_callback() {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
    glDeleteBuffers(1, &Buffers);
    glDeleteVertexArrays(1, &VAOs);
}

void keyboard_callback(unsigned char key, int x, int y) {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
    fprintf(stdout, "key:%c\n", key);
    switch (key) {
    case 'b':
    case 'B': {
        if (blue > 0) {
            blue -= 0.1;
        }
        fprintf(stdout, "blue:%f\n", blue);

        program->uniform("blue", blue);
        glutPostRedisplay();
    } break;
    case 'c':
    case 'C':
        start = (start + 3) % 6;
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAOs);
        glDrawArrays(GL_TRIANGLES, start, 3);
        glutSwapBuffers();
        break;
    case 'g':
    case 'G': {
        if (green > 0) {
            green -= 0.1;
        }
        fprintf(stdout, "green:%f\n", green);

        program->uniform("green", green);
        glutPostRedisplay();
    } break;
    case 'r':
    case 'R': {
        if (red > 0) {
            red -= 0.1;
        }
        fprintf(stdout, "red:%f\n", red);

        program->uniform("red", red);
        glutPostRedisplay();
    } break;
    default:
        break;
    }
}

void keyboard_up_callback(unsigned char key, int x, int y) {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
}

void mouse_callback(int button, int state, int x, int y) {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_UP) {
            x_off += 0.1;
            fprintf(stdout, "x_off:%f\n", x_off);

            program->uniform("x_offset", x_off);
            glutPostRedisplay();
        }

        break;
    case GLUT_MIDDLE_BUTTON:
        fprintf(stdout, "%s_%d\n", __func__, __LINE__);
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_UP) {
            y_off += 0.2;
            fprintf(stdout, "y_off:%f\n", y_off);

            program->uniform("y_offset", y_off);
            glutPostRedisplay();
        }
        break;
    }
}

void entry_callback(int state) {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
}

void visibility_callback(int state) {
    fprintf(stdout, "%s_%d\n", __func__, __LINE__);
}

int main(int argc, char **argv) {
    int win1, win2;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
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
