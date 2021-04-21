#ifndef UMD_SIZABLE_HPP
#define UMD_SIZABLE_HPP
namespace botson {
    namespace sdk {
        class sizeable {
        public:
            virtual unsigned int width() = 0;

            virtual unsigned int height() = 0;

            virtual unsigned int units() = 0;
        };
    }
}
#endif //UMD_SIZABLE_HPP
