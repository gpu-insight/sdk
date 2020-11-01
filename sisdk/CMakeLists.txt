include(GNUInstallDirs)

add_library(sisdk SHARED
    src/common.hpp
    src/context.cpp
    src/files.cpp
    src/framebuffer.cpp
    src/matrices.cpp
    src/program.cpp
    src/screen.cpp
    src/shader.cpp
    src/test.hpp
    src/texture.cpp
    src/vertex_index.cpp
)

add_definitions(-DGL_GLEXT_PROTOTYPES)

set_target_properties(sisdk PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)
set(SISDK_PUBLIC_HEADERS
    include/botson_sdk.hpp
    include/camera.hpp
    include/component.hpp
    include/context.hpp
    include/draw_array.hpp
    include/draw_array.tpp
    include/draw_command.hpp
    include/draw_elements.hpp
    include/draw_elements.tpp
    include/drawable.hpp
    include/event.hpp
    include/files.hpp
    include/framebuffer.hpp
    include/functions.hpp
    include/icanvas.hpp
    include/identifiable.hpp
    include/iprezi.h
    include/matrices.hpp
    include/pixel.hpp
    include/program.hpp
    include/renderbuffer.hpp
    include/rendercomponent.hpp
    include/screen.hpp
    include/shader.hpp
    include/sizable.hpp
    include/storage.hpp
    include/texture.hpp
    include/vertex_attrib.hpp
    include/vertex_attrib.tpp
    include/vertex_index.hpp
    include/vgl.hpp
)
set_target_properties(sisdk PROPERTIES PUBLIC_HEADER "${SISDK_PUBLIC_HEADERS}")

target_include_directories(sisdk PRIVATE include)
target_include_directories(sisdk PRIVATE src)
configure_file(sisdk.pc.in sisdk.pc @ONLY)

if (OpenGL_OpenGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(sisdk ${OPENGL_gl_LIBRARY})
    message(STATUS "OpenGL found" ${OPENGL_INCLUDE_DIR} ${OPENGL_gl_LIBRARY})
endif(OpenGL_OpenGL_FOUND)


install(TARGETS sisdk
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SDK
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sisdk.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
