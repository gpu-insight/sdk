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
    auto s = c->onscreen(16, 16);
    auto canv = s->canvas(umd::icanvas::REGULAR);

    std::string vshader = {"#version 430 core\n"
                           "layout (location=0) in vec4 vPosition;\n"
                           "layout (location=1) in vec2 texCoord;\n"
                           "out vec2 tex_coord;\n"
                           "void main() {\n"
                           "    gl_Position = vec4(vPosition);\n"
                           "    tex_coord = texCoord;\n"
                           "}\n"};
    std::string fshader = {"#version 430 core\n"
                           "precision mediump float;\n"
                           "in vec2 tex_coord;\n"
                           "uniform sampler2D tex;\n"
                           "layout (location=0) out vec4 fColor;\n"
                           "void main() {\n"
                           "    fColor = texture(tex, tex_coord);\n"
                           "}\n"};

    Program *prog = Program::from(Shader::source(GL_VERTEX_SHADER, vshader),
                                  Shader::source(GL_FRAGMENT_SHADER, fshader));
    prog->activate();

    auto position =
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
    auto tex_coord =
        std::make_shared<umd::vertex_attrib<GLfloat>>("texCoord", 2,
                                                      std::vector<GLfloat>{
                                                          -1.0,
                                                          -1.0,
                                                          1.0,
                                                          -1.0,
                                                          1.0,
                                                          1.0,
                                                          1.0,
                                                          1.0,
                                                          -1.0,
                                                          1.0,
                                                          -1.0,
                                                          -1.0,
                                                      });

    unsigned char texture_data[] = {
        0xFF, 0x00, 0xEF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xEF,
        0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00,
        0xEF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xEF, 0xFF, 0xFF,
        0x00, 0xEF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xEF, 0xFF,
        0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xEF,
        0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xEF, 0xFF,
    };

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint tex[3];
    glGenTextures(3, tex);

    /* Immutable 2D texture */
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, tex[0]));
    GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 3,
                            GL_RGBA8, /* SIZED internal format */
                            4, 4));
    for (int level = 0; level < 3; level++) {
        GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, 4 >> level,
                                 4 >> level, GL_RGBA, GL_UNSIGNED_BYTE,
                                 texture_data));
    }

    // texture size is 4x4 while window size is 800x600,
    // so we must tell OpenGL how to handle the magnification.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const shared_ptr<Program> &pp = std::shared_ptr<Program>(prog);
    auto command = std::make_shared<umd::draw_array<GLfloat>>(
        canv, pp, 6, position, tex_coord);
    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    /* Create tex[1] which is a view of tex[0] */
    GL_CHECK(glTextureView(tex[1],                /* another texture name */
                           GL_TEXTURE_2D, tex[0], /* origtexture name */
                           GL_RGBA8, 1,
                           2,      /* start with the second mipmap of tex[0] */
                           0, 1)); /* 2D texture no layers  */

    /* Create tex[2] which is a view of tex[1] */
    GL_CHECK(glTextureView(tex[2],                /* another texture name */
                           GL_TEXTURE_2D, tex[1], /* origtexture name */
                           GL_RGBA8,
                           /* start with the second mipmap of tex[1]
                            * which contains only 2 levels
                            * now tex[2] contains a single level.
                            */
                           1, 1, 0, 1)); /* 2D texture no layers  */

    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    glBindTexture(GL_TEXTURE_2D, tex[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    return 0;
}
