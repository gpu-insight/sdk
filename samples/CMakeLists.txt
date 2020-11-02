find_package(GLUT)
find_package(X11)

include_directories(include)

add_definitions(-DGL_GLEXT_PROTOTYPES)

add_subdirectory(data)
add_subdirectory(include)

if (OpenGL_EGL_FOUND)
    add_subdirectory(egl)
endif(OpenGL_EGL_FOUND)

if (GLUT_FOUND)
    add_subdirectory(glut)
endif(GLUT_FOUND)

if (X11_FOUND)
    add_subdirectory(glx)
endif(X11_FOUND)