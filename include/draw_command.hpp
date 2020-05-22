#ifndef UMD_DRAW_COMMAND_H
#define UMD_DRAW_COMMAND_H
#pragma once

namespace botson {
    namespace sdk {
        class drawable;

        class draw_command {
        public:
            enum primitive {TRIANGLE, TRIANGLE_STRIP, QUADS};
        public:
            virtual void applied(drawable *drawable) = 0;
        };
    }
} // namespace umd

#endif//UMD_DRAW_COMMAND_H
