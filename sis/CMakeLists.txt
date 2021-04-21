include(GNUInstallDirs)

set(HEADER_LIST
    ../include/sis/botson_sdk.hpp
    ../include/sis/camera.hpp
    ../include/sis/component.hpp
    ../include/sis/context.hpp
    ../include/sis/draw_array.hpp
    ../include/sis/draw_array.tpp
    ../include/sis/draw_command.hpp
    ../include/sis/draw_elements.hpp
    ../include/sis/draw_elements.tpp
    ../include/sis/drawable.hpp
    ../include/sis/event.hpp
    ../include/sis/files.hpp
    ../include/sis/framebuffer.hpp
    ../include/sis/functions.hpp
    ../include/sis/icanvas.hpp
    ../include/sis/identifiable.hpp
    ../include/sis/iprezi.h
    ../include/sis/matrices.hpp
    ../include/sis/pixel.hpp
    ../include/sis/program.hpp
    ../include/sis/renderbuffer.hpp
    ../include/sis/rendercomponent.hpp
    ../include/sis/screen.hpp
    ../include/sis/shader.hpp
    ../include/sis/sizable.hpp
    ../include/sis/storage.hpp
    ../include/sis/texture.hpp
    ../include/sis/vertex_attrib.hpp
    ../include/sis/vertex_attrib.tpp
    ../include/sis/vertex_index.hpp
    ../include/sis/vgl.hpp
)

find_package(OpenGL REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

add_library(sis SHARED
    common.hpp
    context.cpp
    files.cpp
    framebuffer.cpp
    matrices.cpp
    program.cpp
    screen.cpp
    shader.cpp
    texture.cpp
    vertex_index.cpp
)

set_target_properties(sis PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)

target_include_directories(sis PUBLIC ../include)
target_link_libraries(sis PRIVATE ${OPENGL_gl_LIBRARY}
    ${OPENGL_egl_LIBRARY}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARIES})
target_compile_features(sis PUBLIC cxx_std_11)
target_compile_definitions(sis PRIVATE GL_GLEXT_PROTOTYPES)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)

configure_file(sis.pc.in sis.pc @ONLY)

install(TARGETS sis
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sis.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
