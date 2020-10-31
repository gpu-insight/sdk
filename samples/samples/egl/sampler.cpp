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
#include <SDK/texture.hpp>
#include <SDK/vertex_attrib.hpp>
#include <unistd.h>

namespace umd = botson::sdk;

int main(int argc, char **argv) {
    auto c = umd::context::create(umd::context::api::OPENGL_ES_2);
    auto s = c->onscreen(1024, 1024);
    auto canv = s->canvas(umd::icanvas::REGULAR);

    std::string vshader = {"#version 430 core\n"
                           "layout (location=0) in vec4 vPosition;\n"
                           "layout (location=1) in vec2 texCoord;\n"
                           "out vec2 tex_coord;\n"
                           "void main() {\n"
                           "    gl_Position = vec4(vPosition);\n"
                           "    tex_coord = texCoord;\n"
                           "}\n"};
    std::string fshader = {
        "#version 430 core\n"
        "precision mediump float;\n"
        "in vec2 tex_coord;\n"
        "uniform sampler2D tex;\n"
        "layout (location=0) out vec4 fColor;\n"
        "void main() {\n"
        /* "    fColor = texture(tex, tex_coord, -16.f);\n" */
        "    fColor = texture(tex, tex_coord);\n"
        "}\n"};

    Program *prog = Program::from(Shader::source(GL_VERTEX_SHADER, vshader),
                                  Shader::source(GL_FRAGMENT_SHADER, fshader));
    prog->activate();

    auto position1 =
        std::make_shared<umd::vertex_attrib<GLfloat>>("vPosition", 3,
                                                      std::vector<GLfloat>{
                                                          -1.0,
                                                          -1.0,
                                                          0.0,
                                                          1.0,
                                                          -1.0,
                                                          0.0,
                                                          1.0,
                                                          1.0,
                                                          0.0,
                                                          1.0,
                                                          1.0,
                                                          0.0,
                                                          -1.0,
                                                          1.0,
                                                          0.0,
                                                          -1.0,
                                                          -1.0,
                                                          0.0,
                                                      });
    auto position2 =
        std::make_shared<umd::vertex_attrib<GLfloat>>("vPosition", 3,
                                                      std::vector<GLfloat>{
                                                          -1.0 / 2,
                                                          -1.0 / 2,
                                                          0.0,
                                                          1.0 / 2,
                                                          -1.0 / 2,
                                                          0.0,
                                                          1.0 / 2,
                                                          1.0 / 2,
                                                          0.0,
                                                          1.0 / 2,
                                                          1.0 / 2,
                                                          0.0,
                                                          -1.0 / 2,
                                                          1.0 / 2,
                                                          0.0,
                                                          -1.0 / 2,
                                                          -1.0 / 2,
                                                          0.0,
                                                      });
    auto position3 =
        std::make_shared<umd::vertex_attrib<GLfloat>>("vPosition", 3,
                                                      std::vector<GLfloat>{
                                                          -1.0 / 16,
                                                          -1.0 / 16,
                                                          0.0,
                                                          1.0 / 16,
                                                          -1.0 / 16,
                                                          0.0,
                                                          1.0 / 16,
                                                          1.0 / 16,
                                                          0.0,
                                                          1.0 / 16,
                                                          1.0 / 16,
                                                          0.0,
                                                          -1.0 / 16,
                                                          1.0 / 16,
                                                          0.0,
                                                          -1.0 / 16,
                                                          -1.0 / 16,
                                                          0.0,
                                                      });
    auto tex_coord1 =
        std::make_shared<umd::vertex_attrib<GLfloat>>("texCoord", 2,
                                                      std::vector<GLfloat>{
                                                          1.0,
                                                          1.0,
                                                          0.0,
                                                          1.0,
                                                          0.0,
                                                          0.0,
                                                          0.0,
                                                          0.0,
                                                          1.0,
                                                          0.0,
                                                          1.0,
                                                          1.0,
                                                      });

    GL_CHECK(glClearColor(1.0f, 0.0f, 0.0f, 0.0f));
    glClear(GL_COLOR_BUFFER_BIT);

    Texture::create_texture_from_image(GL_TEXTURE_2D, "asset/sky_rt.jpg");

    GLuint sampler;
    glGenSamplers(1, &sampler);

    GL_CHECK(glBindSampler(0, sampler));

    GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_MIN_LOD, 0));
    GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_MAX_LOD, 11));
    GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_LOD_BIAS, -16));

    const shared_ptr<Program> &pp = std::shared_ptr<Program>(prog);

    auto command1 = std::make_shared<umd::draw_array<GLfloat>>(
        canv, pp, 6, position1, tex_coord1);
    canv->apply(command1);
    c->swap_buffer();

    sleep(5);

    GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_LOD_BIAS, -16));

    auto command2 = std::make_shared<umd::draw_array<GLfloat>>(
        canv, pp, 6, position2, tex_coord1);
    canv->apply(command2);
    c->swap_buffer();

    sleep(5);

    GL_CHECK(glSamplerParameteri(sampler, GL_TEXTURE_LOD_BIAS, -31));
    auto command3 = std::make_shared<umd::draw_array<GLfloat>>(
        canv, pp, 6, position3, tex_coord1);
    canv->apply(command3);
    c->swap_buffer();

    sleep(5);

    return 0;
}
