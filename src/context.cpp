#include "common.hpp"
#include "context.hpp"
#include "screen.hpp"

namespace umd = botson::sdk;

std::shared_ptr<umd::context> umd::context::create(umd::context::api v) {
    return std::make_shared<umd::context>(v);
}

void umd::context::enable(GLenum option) {
    glEnable(option);
    GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
        printf("glGetError() = %i (0x%.8x) at %s:%i after enable option: %x\n", glError, glError, __FILE__, __LINE__, option);
        exit(1);
    }
}

std::shared_ptr<umd::context> umd::context::xhis() {
    return shared_from_this();
}

Display *getXDisplay() {
    Display *xdisplay = XOpenDisplay(NULL);
    if (xdisplay == nullptr) {
        std::cerr << "Can not connect to the X server" << std::endl;

    }
    return xdisplay;
}

std::shared_ptr<umd::screen> umd::context::onscreen(int width, int height) {
    Display *xdisplay = getXDisplay();
    assert(xdisplay != nullptr);
    _display = eglGetDisplay(xdisplay);

    Window xwindow;

    EGLint numberOfConfigs = 0;

    EGLint configAttributes[] = {
            EGL_NONE,
    };

    EGLint maxVersion, minVersion;
    EGLConfig config;

    bool success = eglInitialize(_display, &maxVersion, &minVersion);
    if (success != EGL_TRUE) {
        printf("eglInitialize Error\n");
        exit(-1);
    } else {
        printf("[EGL] eglInitialize OK! EGL version: v%d.%d\n", maxVersion, minVersion);
    }

    // The ARB_depth_clamp is not supported under the gles
    eglBindAPI(this->_api == api::OPENGL? EGL_OPENGL_API : EGL_OPENGL_ES_API);

    success = eglChooseConfig(_display, configAttributes, &config, 1, &numberOfConfigs);
    if (success != EGL_TRUE) {
        printf("[EGL] eglChooseConfig Error\n");
        exit(-1);
    } else {
        printf("[EGL] get config ok (%d)\n", numberOfConfigs);
    }


    auto screen = DefaultScreen(xdisplay);
    int visualID;

    eglGetConfigAttrib(_display, config, EGL_NATIVE_VISUAL_ID, &visualID);
    XVisualInfo visualInfoTemplate;
    visualInfoTemplate.visualid = visualID;
    int numberOfVisuals;
    XVisualInfo *visual = XGetVisualInfo(xdisplay, VisualIDMask, &visualInfoTemplate, &numberOfVisuals);
    if (visual == NULL) {
        printf("Couldn't get X visual info\n");
    }
    assert(visual != NULL);
    Colormap colormap = XCreateColormap(xdisplay, RootWindow(xdisplay, screen), visual->visual, AllocNone);
    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap = colormap;
    windowAttributes.background_pixel = 0xFFFFFFFF;
    windowAttributes.border_pixel = 0;
    windowAttributes.event_mask = StructureNotifyMask | ExposureMask;
    unsigned long mask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

    xwindow = XCreateWindow(xdisplay, RootWindow(xdisplay, screen), 0, 0, width, height,
                            0, visual->depth, InputOutput, visual->visual, mask, &windowAttributes);
    XSizeHints sizeHints;
    sizeHints.flags = USPosition;
    sizeHints.x = 10;
    sizeHints.y = 10;

    XSetStandardProperties(xdisplay, xwindow, "Botson OpenGL SDK", "", None, 0, 0, &sizeHints);
    XMapWindow(xdisplay, xwindow);
    XEvent event;

    XIfEvent(xdisplay, &event, [](Display *display, XEvent *event, char *windowPointer) -> Bool {
        return (event->type == MapNotify && event->xmap.window == (*((Window *) windowPointer)));
    }, (char *) &xwindow);
    XSetWMColormapWindows(xdisplay, xwindow, &xwindow, 1);
    XFlush(xdisplay);

    XSelectInput(xdisplay, xwindow, KeyPressMask | ExposureMask | EnterWindowMask
                                    | LeaveWindowMask | PointerMotionMask | VisibilityChangeMask | ButtonPressMask
                                    | ButtonReleaseMask | StructureNotifyMask);

    assert(xwindow != 0);

    if (_display == EGL_NO_DISPLAY) {
        std::cout << "Can not get EGL Display" << std::endl;
        exit(-1);
    }


    EGLint contextAttributes[] = {
            EGL_CONTEXT_CLIENT_VERSION,
            2,
            EGL_NONE,
    };
    _context = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttributes);

    _surface = eglCreateWindowSurface(_display, config, xwindow, configAttributes);
    assert(_surface != EGL_NO_SURFACE);

    eglMakeCurrent(_display, _surface, _surface, _context);
    assert(EGL_SUCCESS == eglGetError());
    return std::make_shared<umd::screen>(xhis(), width, height);
}


std::shared_ptr<umd::screen> umd::context::offscreen() {
    _display = eglGetDisplay(NULL);
    EGLint maxVersion, minVersion;
    assert(eglInitialize(_display, &maxVersion, &minVersion) == EGL_TRUE);

    eglBindAPI(this->_api == api::OPENGL? EGL_OPENGL_API : EGL_OPENGL_ES_API);
    EGLint configAttributes[] = {
            EGL_ALPHA_SIZE, 8,
            EGL_NONE,
    };

    assert(eglChooseConfig(_display, configAttributes, &config, 1,
                           &numberOfConfigs) ==
           EGL_TRUE);

    EGLint pbufferAtribs[5] = {EGL_WIDTH, 800, EGL_HEIGHT, 600, EGL_NONE};
    _surface = eglCreatePbufferSurface(_display, config, pbufferAtribs);
    assert(_surface != EGL_NO_SURFACE);

    EGLint contextAttributes[] = {
            EGL_CONTEXT_CLIENT_VERSION,
            _api == api::OPENGL_ES_1 ? 1 : 2,
            EGL_NONE,
    };

    _context =
            eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttributes);
    assert(_surface != EGL_NO_SURFACE);

    assert(eglMakeCurrent(_display, _surface, _surface, _context) == EGL_TRUE);
    assert(EGL_SUCCESS == eglGetError());
    return std::make_shared<umd::screen>(xhis());
}

umd::context::~context() {
    eglBindAPI(this->_api == api::OPENGL? EGL_OPENGL_API : EGL_OPENGL_ES_API);
    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, _context);
    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_display, _context);
    eglDestroyContext(_display, _surface);
    eglTerminate(_display);
}

umd::context::context(api a):_api(a) {

}

void umd::context::swap_buffer() {
    eglSwapBuffers(_display, _surface);
}

bool umd::context::check_error(const char *file, int line)
{
    GLenum errorCode;
    std::string errorStr;
    bool error = false;

    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  errorStr = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 errorStr = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             errorStr = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                errorStr = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               errorStr = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 errorStr = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: errorStr = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }

        std::cout << errorStr << " | " << file << " (" << line << ")" << std::endl;
        error = true;
    }

    return error;
}
