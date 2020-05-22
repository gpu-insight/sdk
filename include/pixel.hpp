#ifndef UMD_PIXEL_HPP
#define UMD_PIXEL_HPP

namespace botson {
    namespace sdk {
        class pixel {
        private:
            GLenum _format;
            unsigned char _r, _g, _b, _a;

        public:
            pixel() {}

            pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
                _r = r;
                _g = g;
                _b = b;
                _a = a;
            }

            GLenum format() { return _format; };

            unsigned char r() { return _r; };
            unsigned char g() { return _g; };
            unsigned char b() { return _b; };

            unsigned char a() { return _a; };
        };
    }
}
#endif //UMD_PIXEL_HPP
