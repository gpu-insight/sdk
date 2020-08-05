#include "common.hpp"
#include "draw_command.hpp"
#include "framebuffer.hpp"
#include "pixel.hpp"


namespace umd = botson::sdk;

void umd::framebuffer::apply(std::shared_ptr<draw_command> draw_command) {
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    draw_command->applied(this);
}


umd::framebuffer::framebuffer(const std::shared_ptr<umd::screen> &scr, unsigned int id) {
    _screen = scr;
    _id = id;
}

std::vector<umd::pixel> umd::framebuffer::pixels(int x, int y, int width, int height,
                                                       GLenum format, GLenum type) {
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    auto *p = (umd::pixel*)malloc(sizeof(umd::pixel) * width * height);
    std::vector<umd::pixel> ps;
    GLubyte image[1][width][height][4];
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);

    for (int w = 0; w < width; w++) {
        for (int h = 0; h < height; h++) {
            GLubyte r = image[0][w][h][0];
            GLubyte g = image[0][w][h][1];
            GLubyte b = image[0][w][h][2];
            GLubyte a = image[0][w][h][3];
            ps.emplace_back(pixel(r, g, b, a));
            //*(p + w*height + height) = pixel(r, g, b, a);
        }
    }

    return ps;
}

std::shared_ptr<umd::framebuffer> umd::framebuffer::attach(std::shared_ptr<umd::renderbuffer> rbo) {
    umd::ensure(glBindFramebuffer)(GL_FRAMEBUFFER, std::forward<GLuint>(_id));
    umd::ensure(glFramebufferRenderbuffer)(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                           GL_RENDERBUFFER, rbo->id());
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return NULL;
    }
    _renderbuffer = rbo;
    return xhis();
};

std::shared_ptr<umd::framebuffer> umd::framebuffer::blit() {
    auto srbo = renderbuffer::create();
    srbo->attach(storage(512, 512, 1));
    auto sfbo = framebuffer::create();
    sfbo->attach(srbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sfbo->id());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
    glBlitFramebuffer(0, 0, 512, 512, 0, 0, 512, 512, GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);
    return sfbo;
}

std::shared_ptr<umd::framebuffer> umd::framebuffer::create() {
    GLuint fid;
    glGenFramebuffers(1, &fid);
    return std::make_shared<framebuffer>(fid);
}

std::shared_ptr<umd::framebuffer> umd::framebuffer::xhis() {
    return shared_from_this();
}

unsigned int umd::framebuffer::id() {
    return _id;
}

umd::framebuffer::framebuffer(unsigned int id) {
    _id = id;
};
