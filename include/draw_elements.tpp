#include "program.hpp"
#include "matrices.hpp"
#include "draw_elements.hpp"

namespace umd = botson::sdk;

template<typename T>
void umd::draw_elements<T>::applied(umd::drawable *drawable) {
    _program->activate();
    _program->uniform("mvp", Matrices::instance()->mvp());

    glBindVertexArray(m_vao);
    for (const auto& v: _vattribs) {
        glEnableVertexAttribArray(_program->attribute(v->name()));
        glVertexAttribPointer(_program->attribute(v->name()), v->dimensions(), GL_FLOAT, GL_FALSE, 0, 0);
    }

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
    glDrawElements(glprimitive(_primitive), 24, GL_UNSIGNED_SHORT, NULL);
}

template<typename T>
umd::draw_elements<T>::draw_elements(std::shared_ptr<icanvas> canvas, std::shared_ptr<Program> program,
                                  umd::draw_command::primitive p, umd::vertex_index indices,
                                  std::shared_ptr<vertex_attrib<T>> position) : _indices(indices) {
    _vattribs.push_back(position);
    _program = program;
    _canvas = canvas;
    _primitive = p;
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
    glBufferStorage(GL_ARRAY_BUFFER, position->size(), position->data(), 0);

    glGenBuffers(1, &_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), indices.data(), GL_STATIC_DRAW);
}

template<typename T>
GLenum umd::draw_elements<T>::glprimitive(umd::draw_command::primitive p) {
    switch (p) {
        case QUADS:
            return GL_QUADS;
        case TRIANGLE:
            return GL_TRIANGLES;
        case TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        default:
            return GL_TRIANGLES;
    }
}
