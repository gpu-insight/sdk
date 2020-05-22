#ifndef UMD_FUNCTIONS_HPP
#define UMD_FUNCTIONS_HPP
namespace botson {
    namespace sdk {
        template<typename T, typename...Args>
        std::unique_ptr<T> make_unique(Args && ...args) {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }

        template<typename>
        class Ensurer;

        template<typename R, typename... T>
        class Ensurer<R(T...)> {
            typedef R (*func_type)(T...);

            func_type _f;
        public:
            Ensurer(func_type f) : _f(f) {};

            R operator()(T &&...args) {
                R r = _f(std::forward<T>(args)...);
                GLenum glError = glGetError();
                if (glError != GL_NO_ERROR) {
                    printf("glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__);
                    exit(1);
                }
                return r;
            }
        };

        template<typename... T>
        class Ensurer<void(T...)> {
            typedef void (*func_type)(T...);
            func_type _f;
        public:
            Ensurer(func_type f) : _f(f) {};

            void operator()(T &&...args) {
                _f(std::forward<T>(args)...);
                GLenum glError = glGetError();
                if (glError != GL_NO_ERROR) {
                    printf("glGetError() = %i (0x%.8x)", glError, glError);
                    exit(1);
                }
            }

            void operator()(T& ...args) {
                _f(std::forward<T>(args)...);
                GLenum glError = glGetError();
                if (glError != GL_NO_ERROR) {
                    printf("glGetError() = %i (0x%.8x)", glError, glError);
                    exit(1);
                }
            }
        };

        template<typename ...T>
        Ensurer<void(T...)> ensure(void(*fp)(T...args)) {
            Ensurer<void(T...)> f(fp);
            return f;
        }
    }
}
#endif //UMD_FUNCTIONS_HPP
