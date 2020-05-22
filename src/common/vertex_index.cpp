#include "common.hpp"
#include "vertex_index.hpp"

namespace umd = botson::sdk;

umd::vertex_index::vertex_index(std::vector<unsigned short> indices) {
    _indices = std::move(indices);
}

unsigned int umd::vertex_index::length() {
    return _indices.size();
}

unsigned int umd::vertex_index::size() {
    return _indices.size() * sizeof(unsigned int);
}

void *umd::vertex_index::data() {
    return _indices.data();
}

