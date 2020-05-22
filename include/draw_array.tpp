#include <program.hpp>
#include "context.hpp"
#include "draw_array.hpp"
#include "../src/common/common.hpp"


namespace umd = botson::sdk;

template<typename T>
void umd::draw_array<T>::applied(umd::drawable *drawable) {
    for (const auto& v: vattribs) {
        GL_CHECK(glEnableVertexAttribArray(_program->attribute(v->name())));
        GL_CHECK(glVertexAttribPointer(_program->attribute(v->name()), v->dimensions(), GL_FLOAT, GL_FALSE, 0, v->data()));
    }
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, _count));
}

template<typename T>
template<typename... Attribs>
umd::draw_array<T>::draw_array(std::shared_ptr <icanvas>, std::shared_ptr<Program> prog, int count, Attribs... attribs) {
    _count = count;

    const int size = sizeof...(attribs);
    std::shared_ptr<vertex_attrib<T>> ats[size] = {attribs...};

    for (int i = 0; i < size; i++) {
        vattribs.push_back(ats[i]);
    }
}

template<typename T>
umd::draw_array<T>::draw_array() {
    _count = 0;
}

template<typename T>
umd::draw_array<T>::draw_array(std::shared_ptr<icanvas>, std::shared_ptr<Program> prog, int count, std::shared_ptr<vertex_attrib<T>> position,
                            std::shared_ptr<vertex_attrib<T>> color) {
    _program = prog;
    _count = count;
    vattribs.push_back(position);
    vattribs.push_back(color);
}

template<typename T>
umd::draw_array<T>::draw_array(std::shared_ptr<icanvas> canv, std::shared_ptr<Program> prog, int count,
                            std::shared_ptr<vertex_attrib<T>> position) {
    _program = prog;
    _count = count;
    vattribs.push_back(position);
}
