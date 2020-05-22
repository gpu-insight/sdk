#ifndef UMD_INDEX_HPP
#define UMD_INDEX_HPP

namespace botson {
    namespace sdk {
        class vertex_index {
        private:
            std::vector<unsigned short> _indices;
            GLuint _index_buffer;
        public:
            explicit vertex_index(std::vector<unsigned short> indices);

            unsigned int size();
            unsigned int length();

            void *data();
        };
    }
}

#endif //UMD_ATTRIBUTES_HPP
