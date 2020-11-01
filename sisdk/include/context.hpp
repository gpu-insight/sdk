#ifndef UMD_CONTEXT_HPP
#define UMD_CONTEXT_HPP
#include <memory>

namespace botson {
    namespace sdk {
        class screen;

        class context : public std::enable_shared_from_this<context> {
        public:
            enum class api {
                OPENGL_ES_1 = 1,
                OPENGL_ES_2 = 2,
                OPENGL,
            };
        private:
            std::shared_ptr<EGLDisplay> __display;
            std::shared_ptr<EGLSurface> __surface;
            std::shared_ptr<EGLContext> __context;
            EGLDisplay _display;
            EGLSurface _surface;
            EGLContext _context;
            EGLConfig config;
            EGLint numberOfConfigs = 0;
            api _api;

        public:

            static std::shared_ptr<context> create(api v);

            void enable(GLenum option);

            std::shared_ptr<context> xhis();

            std::shared_ptr<screen> offscreen();

            std::shared_ptr<screen> onscreen(int width = 512, int height = 512);

            void swap_buffer();

            static bool check_error(const char *file, int line);

            ~context();

        public:
            context(api);
        };
    }
}


#define GL_CHECK(x) \
    x; \
    { \
        if (umd::context::check_error(__FILE__, __LINE__)) \
            exit(1); \
    }

#endif //UMD_CONTEXT_HPP
