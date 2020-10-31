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
        0xFF, 0x00, 0xEF, 0xFF, 0xFF, 0x00, 0xEF, 0xFF, 0xFF, 0x00, 0xEF,
        0xFF, 0xFF, 0x00, 0xEF, 0xFF, 0xFF, 0x00, 0xEF, 0xFF, 0xFF, 0x00,
        0xEF, 0xFF, 0xFF, 0x00, 0xEF, 0xFF, 0xFF, 0x00, 0xEF, 0xFF, 0xFF,
        0x00, 0xEF, 0xFF, 0xFF, 0x00, 0xEF, 0xFF, 0xFF, 0x00, 0xEF, 0xFF,
        0xFF, 0x00, 0xEF, 0xFF, 0xFF, 0x00, 0xEF, 0xFF, 0xFF, 0x00, 0xEF,
        0xFF, 0xFF, 0x00, 0xEF, 0xFF, 0xFF, 0x00, 0xEF, 0xFF,
    };

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 texture_data);

    // texture size is 4x4 while window size is 800x600,
    // so we must tell OpenGL how to handle the minification.

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // clipped while DEPTH_CLAMP disabled
    const shared_ptr<Program> &pp = std::shared_ptr<Program>(prog);
    auto command = std::make_shared<umd::draw_array<GLfloat>>(
        canv, pp, 6, position, tex_coord);

    // original
    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    const int width = 800;
    const int height = 600;
    const int bytes_per_pixel = 4;

    void *data = malloc(bytes_per_pixel * width * height * sizeof(GLubyte));

    glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // only get blue channel, because green is zero
    static const GLint swizzles1[] = {GL_GREEN, GL_GREEN, GL_BLUE, GL_ALPHA};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles1);

    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // replace alpha channel with green so that it is transparent
    static const GLint swizzles2[] = {GL_BLUE, GL_ALPHA, GL_ZERO, GL_GREEN};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles2);

    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // replace red channel with blue and green channel with alpha
    static const GLint swizzles3[] = {GL_BLUE, GL_ALPHA, GL_ZERO, GL_ALPHA};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles2);

    canv->apply(command);
    c->swap_buffer();
    sleep(2);

    glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, data);

    free(data);

    return 0;
}
