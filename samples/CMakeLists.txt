find_package(GLUT)
find_package(X11)

if (OpenGL_EGL_FOUND)
    add_subdirectory(samples/egl)
endif(OpenGL_EGL_FOUND)

if (GLUT_FOUND)
    add_subdirectory(samples/glut)
endif(GLUT_FOUND)

if (X11_FOUND)
    add_subdirectory(samples/glx)
endif(X11_FOUND)
