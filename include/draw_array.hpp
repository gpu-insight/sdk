#ifndef UMD_DRAW_ARRAY_HPP
#define UMD_DRAW_ARRAY_HPP
class Program;
#include "draw_command.hpp"
#include "../../../include/icanvas.hpp"
#include "vertex_attrib.hpp"

namespace botson {
    namespace sdk {
        template <typename T>
        class draw_array : public draw_command {
        private:
            int _count;
        public:
            void applied(drawable *drawable) override;

            template <typename...Attribs>
            draw_array(std::shared_ptr<icanvas>, std::shared_ptr<Program>, int count, Attribs... attribs);
            draw_array();

            draw_array(std::shared_ptr<icanvas>, std::shared_ptr<Program>, int count, std::shared_ptr<vertex_attrib<T>>,
                       std::shared_ptr<vertex_attrib<T>>);
            draw_array(std::shared_ptr<icanvas>, std::shared_ptr<Program>, int count, std::shared_ptr<vertex_attrib<T>>);

            std::vector<std::shared_ptr<vertex_attrib<T>>> vattribs;
            std::shared_ptr<Program> _program;
        };
    }
}

#include "../draw_array.tpp"
#endif //UMD_DRAW_ARRAY_HPP
