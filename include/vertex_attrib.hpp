#ifndef UMD_ATTRIBUTES_HPP
#define UMD_ATTRIBUTES_HPP

namespace botson {
    namespace sdk {
        template<typename T>
        class vertex_attrib {
        private:
            std::string _name;
            std::vector<T> _attribs;
            int _dimensions;
        public:
            vertex_attrib(const std::string& name, int dimensions, std::vector<T> attribs);
            std::string name();
            int dimensions();
            unsigned int size();
            const void * data();
        };
    }
}

#include "vertex_attrib.tpp"
#endif //UMD_ATTRIBUTES_HPP
