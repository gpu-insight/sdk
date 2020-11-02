add_executable(glxgears glxgears.cpp save2bmp.cpp)

target_link_libraries(glxgears ${X11_LIBRARIES} ${OPENGL_gl_LIBRARY} ${SISDK_LIBRARIES})
target_include_directories(glxgears PUBLIC ${X11_INCLUDE_DIR}
        ${OPENGL_INCLUDE_DIR}
        ${SISDK_INCLUDE_DIR})

install(TARGETS glxgears
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
