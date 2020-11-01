include_directories(${X11_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} .)

add_executable(glxgears glxgears.cpp save2bmp.cpp)

target_link_libraries(glxgears ${X11_LIBRARIES} ${OPENGL_gl_LIBRARY} sisdk)
