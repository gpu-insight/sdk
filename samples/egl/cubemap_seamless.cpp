#include <EGL/egl.h>
#include <GL/gl.h>
#include <unistd.h>
#include <string>
#include <SDK/program.hpp>
#include <SDK/context.hpp>
#include <SDK/icanvas.hpp>
#include <SDK/screen.hpp>
#include <SDK/matrices.hpp>
#include <SDK/vertex_attrib.hpp>
#include <SDK/draw_elements.hpp>

#define TEXTURE_WIDTH 512
#define TEXTURE_HEIGHT 512

namespace umd = botson::sdk;

int main(int argc, char **argv) {
    auto c = umd::context::create(umd::context::api::OPENGL_ES_2);
    auto s = c->onscreen();
    auto canv = s->canvas(umd::icanvas::REGULAR);

    // The 320 es is not supported by the mesa
    std::string vertexShader = {"#version 330\n"
                                "uniform mat4 mvp;"
                                "layout (location = 0) in vec4 position;"
                                "out vec3 tex_coor;"
                                "void main(void) {\n"
                                "   gl_Position = mvp * position;"
                                "   tex_coor = position.xyz;"
                                "}"};

    std::string fragShader = {"#version 330\n"
                              "precision mediump float;"
                              "layout (location=0) out vec4 fColor;"
                              "in vec3 tex_coor;"
                              "uniform samplerCube cube_tex;"
                              "void main(void) {"
                              "    fColor = texture(cube_tex, tex_coor);"
                              "}"};

    Program *p = Program::from(Shader::source(GL_VERTEX_SHADER, vertexShader),
                               Shader::source(GL_FRAGMENT_SHADER, fragShader));
    p->activate();
    auto position = std::make_shared<umd::vertex_attrib<GLfloat>>(
        "position", 3,
        std::vector<GLfloat>{-128.0f, -128.0f, -128.0f, -128.0f, -128.0f,
                             128.0f,  -128.0f, 128.0f,  -128.0f, -128.0f,
                             128.0f,  128.0f,  128.0f,  -128.0f, -128.0f,
                             128.0f,  -128.0f, 128.0f,  128.0f,  128.0f,
                             -128.0f, 128.0f,  128.0f,  128.0f});

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glClearColor(0.0f, 1.0f, 0.0f, 0.0f));
    GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

    GLubyte nx_black_texture[TEXTURE_HEIGHT][TEXTURE_WIDTH][4];
    GLubyte px_black_texture[TEXTURE_HEIGHT][TEXTURE_WIDTH][4];
    GLubyte ny_white_texture[TEXTURE_HEIGHT][TEXTURE_WIDTH][4];
    GLubyte py_white_texture[TEXTURE_HEIGHT][TEXTURE_WIDTH][4];
    GLubyte nz_white_texture[TEXTURE_HEIGHT][TEXTURE_WIDTH][4];
    GLubyte pz_white_texture[TEXTURE_HEIGHT][TEXTURE_WIDTH][4];

    for (auto row = 0; row < TEXTURE_HEIGHT; row++) {
        for (auto col = 0; col < TEXTURE_WIDTH; col++) {
            for (int rgb = 0; rgb < 4; rgb++) {
                nx_black_texture[row][col][rgb] = rgb == 3 ? 255 : 0;
                px_black_texture[row][col][rgb] = rgb == 3 ? 255 : 0;
                ny_white_texture[row][col][rgb] = rgb == 3 ? 255 : 128;
                py_white_texture[row][col][rgb] = rgb == 3 ? 255 : 128;
                nz_white_texture[row][col][rgb] = 255;
                pz_white_texture[row][col][rgb] = 255;
            }
        }
    }

    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tid);
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8,
                          TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, px_black_texture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8,
                          TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, nx_black_texture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8,
                          TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, py_white_texture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8,
                          TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, ny_white_texture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8,
                          TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, pz_white_texture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8,
                          TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, nz_white_texture));

    GL_CHECK(
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                             GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                             GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                             GL_CLAMP_TO_EDGE));

    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glViewport(0, 0, 512, 512);
    Matrices *matrices = Matrices::instance();

    matrices->rotate(45.0f, 0, 1, 0);
    matrices->lookat(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1.0f),
                     glm::vec3(0, 1, 0));
    matrices->perspective(150.0f, 1.0f, 1.0f, 5000.0f);

    //    matrices->ortho(-256.0f, 256.0f, -256.0f, 256.0f);
    //    matrices->lookat(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1.0f),
    //    glm::vec3(0, 1, 0));

    const shared_ptr<Program> &pp = std::shared_ptr<Program>(p);
    umd::vertex_index trip1(
        std::vector<unsigned short>{0, 1, 3, 2, 4, 6, 7, 5, 2, 3, 7, 6,
                                    0, 4, 5, 1, 0, 2, 6, 4, 1, 5, 7, 3});

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tid);
    canv->apply(std::make_shared<umd::draw_elements<GLfloat>>(
        canv, pp, umd::draw_command::QUADS, trip1, position));

    c->swap_buffer();

    GLubyte data[TEXTURE_HEIGHT][TEXTURE_WIDTH][4];
    memset(&data, 0, TEXTURE_HEIGHT * TEXTURE_WIDTH * 4 * sizeof(GLubyte));

    glReadPixels(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE,
                 &data);
    for (auto row = 0; row < TEXTURE_HEIGHT; row++) {
        for (auto col = 0; col < TEXTURE_WIDTH; col++) {
            for (int rgb = 0; rgb < 4; rgb++) {
                int v = int(data[row][col][rgb]);
                if (v != 255 && v != 0 && v != 128) {
                    std::cout << "success" << std::endl;
                    std::cout << v << std::endl;
                }
            }
        }
    }
    sleep(2);

    return 0;
}
