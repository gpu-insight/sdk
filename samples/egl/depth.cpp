#include <EGL/egl.h>
#include <GL/gl.h>
#include <string>
#include <SDK/program.hpp>
#include <SDK/drawable.hpp>
#include <SDK/context.hpp>
#include <SDK/icanvas.hpp>
#include <SDK/screen.hpp>
#include <SDK/draw_array.hpp>
#include <SDK/matrices.hpp>
#include <SDK/vertex_attrib.hpp>
#include <unistd.h>

namespace umd = botson::sdk;

int main(int argc, char **argv) {
    auto c = umd::context::create(umd::context::api::OPENGL_ES_2);
    auto s = c->onscreen();
    auto canv = s->canvas(umd::icanvas::REGULAR);

    // The 320 es is not supported by the mesa
    std::string vertexShader = {"#version 330\n"
                                "uniform mat4 mvp;"
                                "in vec3 position;"
                                "in vec3 color;"
                                "out vec3 vColor;"
                                "void main(void) {\n"
                                "   gl_Position = mvp * vec4(position, 1);"
                                "   vColor = color;"
                                "}"};

    std::string fragShader = {"#version 330\n"
                              "precision mediump float;"
                              "layout (location=0) out vec4 fColor;"
                              "in vec3 vColor;"
                              "void main(void) {\n"
                              "    fColor = vec4(vColor, 1.0);"
                              "}"};

    Program *p = Program::from(Shader::source(GL_VERTEX_SHADER, vertexShader),
                               Shader::source(GL_FRAGMENT_SHADER, fragShader));
    p->activate();

    auto position =
        std::make_shared<umd::vertex_attrib<GLfloat>>("position", 3,
                                                      std::vector<GLfloat>{
                                                          -1.0,
                                                          0.0,
                                                          0.0,
                                                          1.0,
                                                          0.0,
                                                          0.0,
                                                          0.0,
                                                          10.0,
                                                          -10.0,
                                                      });
    auto color =
        std::make_shared<umd::vertex_attrib<GLfloat>>("color", 3,
                                                      std::vector<GLfloat>{
                                                          1.0,
                                                          0.0,
                                                          0.0,
                                                          0.0,
                                                          1.0,
                                                          0.0,
                                                          0.0,
                                                          0.0,
                                                          1.0,
                                                      });

    // TODO why glShadeModel cause the following glEnable(GL_DEPTH_TEST) fail
    //    glShadeModel(GL_FLAT);

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glClearColor(1.0f, 1.0f, 1.0f, 0.0f));
    GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

    Matrices *matrices = Matrices::instance();
    matrices->lookat(glm::vec3(0, 0, 20), glm::vec3(0, 0, 0),
                     glm::vec3(0, 1, 0));
    matrices->perspective(150.0f, 1.0f, 1.0f, 25.0f);
    p->uniform("mvp", matrices->mvp());

    // clipped while DEPTH_CLAMP disabled
    const shared_ptr<Program> &pp = std::shared_ptr<Program>(p);
    auto command = std::make_shared<umd::draw_array<GLfloat>>(canv, pp, 3,
                                                              position, color);
    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // unclippd while the DEPTH_CLAMP enabled
    c->enable(GL_DEPTH_CLAMP);
    glDepthRange(0, 50.0f);
    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    return 0;
}
