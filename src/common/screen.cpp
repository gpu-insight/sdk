#include "common.hpp"
#include "context.hpp"
#include "screen.hpp"
#include "framebuffer.hpp"

namespace umd = botson::sdk;

std::shared_ptr<umd::icanvas> umd::screen::canvas(umd::icanvas::antialising aa) {
    if (aa == umd::icanvas::MULTISAMPLE) {
        auto mrbo = renderbuffer::create();
        mrbo->attach(storage(512, 512, 4));
        GLuint fid;
        glGenFramebuffers(1, &fid);
        const std::shared_ptr<framebuffer> &ptr = std::make_shared<umd::framebuffer>(xhis(), fid);
        ptr->attach(mrbo);
        return ptr;
    }
    else {
        return std::make_shared<umd::framebuffer>(xhis(), 0);
    }
}


umd::screen::screen(const std::shared_ptr<umd::context>& con, int width, int height) {
    _ctx = con;
    _w = width;
    _h = height;
}

void umd::screen::swap_buffer() {
    const std::shared_ptr<umd::context> &ptr = _ctx.lock();
    ptr->swap_buffer();
}

std::shared_ptr<umd::screen> umd::screen::xhis() {
    return shared_from_this();
}
