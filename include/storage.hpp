#ifndef UMD_STORAGE_HPP
#define UMD_STORAGE_HPP

#include "sizable.hpp"

namespace botson {
    namespace sdk {
        class storage : public sizeable {
        private:
            unsigned int _width, _height, _units;

        public:
            storage(unsigned int width, unsigned int height, unsigned units) {
                _width = width;
                _height = height;
                _units = units;
            }

            unsigned int width() override { return _width; }

            unsigned int height() override { return _height; }

            unsigned int units() override { return _units; }
        };
    }
}

#endif//UMD_STORAGE_HPP