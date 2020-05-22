#pragma once
#include <memory>
#ifndef UMD_DRAWABLE_H
#define UMD_DRAWABLE_H
namespace botson {
    namespace sdk {
        class draw_command;

        class drawable {
        public:
            virtual void apply(std::shared_ptr<draw_command> command) = 0;
        };
    }
} // namespace umd
#endif //UMD_DRAWABLE_H