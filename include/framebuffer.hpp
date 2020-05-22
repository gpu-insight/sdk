#ifndef UMD_FRAMEBUFFER_HPP
#define UMD_FRAMEBUFFER_HPP

#include "icanvas.hpp"
#include "pixel.hpp"
#include "renderbuffer.hpp"
#include "screen.hpp"

namespace botson {
    namespace sdk {
        class framebuffer : public icanvas,
                            public std::enable_shared_from_this<framebuffer>,
                            public identifiable {
        private:
            std::weak_ptr<screen> _screen;
            unsigned int _id;
            std::shared_ptr<renderbuffer> _renderbuffer;
        public:
            unsigned int id() override;

            explicit framebuffer(const std::shared_ptr<screen> &scr, unsigned int id);
            explicit framebuffer(unsigned int id);

            void apply(std::shared_ptr<draw_command> draw_command) override;

            std::shared_ptr<framebuffer> attach(std::shared_ptr<renderbuffer> rbo);

            std::shared_ptr<framebuffer> xhis();

            std::shared_ptr<framebuffer> blit();

            std::shared_ptr<renderbuffer> getRenderBufferObject() { return _renderbuffer; }

            std::vector<pixel> pixels(int x, int y, int width, int height, GLenum format, GLenum type) override;

            static std::shared_ptr<framebuffer> create();
        };
    }
}

#endif //UMD_FRAMEBUFFER_HPP
