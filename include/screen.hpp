#ifndef UMD_SCREEN_HPP
#define UMD_SCREEN_HPP

#include "icanvas.hpp"

namespace botson {
    namespace sdk {
        class context;

        class screen :public std::enable_shared_from_this<screen>  {
        private:
            std::weak_ptr<context> _ctx;
            int _w, _h;

        public:
            std::shared_ptr<icanvas> canvas(icanvas::antialising aa);

            explicit screen(const std::shared_ptr<context>& context, int width = 512, int height = 512);

            std::shared_ptr<screen> xhis();

            void swap_buffer();

            int width() { return _w; };

            int height() { return _h; };
        };
    }
}

#endif //UMD_SCREEN_HPP
