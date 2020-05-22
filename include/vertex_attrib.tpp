#include "vertex_attrib.hpp"

namespace umd = botson::sdk;

template<typename T>
std::string umd::vertex_attrib<T>::name() {
    return _name;
}

template<typename T>
const void *umd::vertex_attrib<T>::data() {
    return _attribs.data();
}

template<typename T>
int umd::vertex_attrib<T>::dimensions() { return _dimensions; }

template<typename T>
umd::vertex_attrib<T>::vertex_attrib(const std::string & name, int dimensions, std::vector<T> attribs) :_attribs(attribs){
    _name = name;
    _dimensions = dimensions;
}

template<typename T>
unsigned int umd::vertex_attrib<T>::size() {
    return _dimensions * _attribs.size() * sizeof(T);
}

