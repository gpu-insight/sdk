#ifndef UMD_ICANVAS_HPP
#define UMD_ICANVAS_HPP

#include "drawable.hpp"

namespace botson {
    namespace sdk {
        class pixel;

        class icanvas : public drawable {
        public:
            enum antialising {REGULAR, MULTISAMPLE};
            virtual std::vector<pixel> pixels(int x, int y, int width, int height,
                                            GLenum format, GLenum type) = 0;
        };
    }
}

#endif //UMD_ICANVAS_HPP
