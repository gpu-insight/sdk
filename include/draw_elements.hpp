#ifndef UMD_DRAW_ELEMENTS_HPP
#define UMD_DRAW_ELEMENTS_HPP

#include "vertex_attrib.hpp"
#include "vertex_index.hpp"
#include "common.hpp"
namespace botson {
    namespace sdk {
        template<typename T>
        class draw_elements : public draw_command {
        private:
            std::vector<std::shared_ptr<vertex_attrib<T>>> _vattribs;
            vertex_index _indices;
            std::shared_ptr<Program> _program;
            std::shared_ptr<icanvas> _canvas;
            draw_command::primitive _primitive;
            GLuint m_vao;
            GLuint _index_buffer;
            GLuint _vertex_buffer;
        private:
            static GLenum glprimitive(draw_command::primitive p);
        public:
            draw_elements(std::shared_ptr<icanvas>, std::shared_ptr<Program>, draw_command::primitive p, vertex_index v,
                    std::shared_ptr<vertex_attrib<T>> position);
            void applied(drawable *drawable) override;

        };
    }
}

#include "../draw_elements.tpp"
#endif //UMD_DRAW_ELEMENTS_HPP
