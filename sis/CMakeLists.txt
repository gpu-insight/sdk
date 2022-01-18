include(GNUInstallDirs)

file(GLOB_RECURSE HEADER_LIST "../include/*.hpp" "../include/*.tpp")

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
set_target_properties(sis PROPERTIES PUBLIC_HEADER "${HEADER_LIST}")

target_include_directories(sis PUBLIC ../include)
target_link_libraries(sis
    PUBLIC ${OPENGL_gl_LIBRARY}
    PRIVATE ${OPENGL_egl_LIBRARY}
    PRIVATE ${JPEG_LIBRARIES}
    PRIVATE ${PNG_LIBRARIES})
target_compile_features(sis PUBLIC cxx_std_11)
target_compile_options(sis PRIVATE "-Wall")
target_compile_definitions(sis PUBLIC GL_GLEXT_PROTOTYPES)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)

configure_file(sis.pc.in sis.pc @ONLY)

install(TARGETS sis
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/sis
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/sis.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
