#ifndef UMD_IDENTIFIABLE_HPP
#define UMD_IDENTIFIABLE_HPP
namespace botson {
    namespace sdk {
        class identifiable {
        public:
            virtual unsigned int id() = 0;
        };
    }
}

#endif //UMD_IDENTIFIABLE_HPP
