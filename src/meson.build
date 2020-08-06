sdk_headers = files(
    '../include/botson_sdk.hpp',
    '../include/camera.hpp',
    '../include/component.hpp',
    '../include/context.hpp',
    '../include/draw_array.hpp',
    '../include/draw_array.tpp',
    '../include/draw_command.hpp',
    '../include/draw_elements.hpp',
    '../include/draw_elements.tpp',
    '../include/drawable.hpp',
    '../include/event.hpp',
    '../include/files.hpp',
    '../include/framebuffer.hpp',
    '../include/functions.hpp',
    '../include/icanvas.hpp',
    '../include/identifiable.hpp',
    '../include/matrices.hpp',
    '../include/pixel.hpp',
    '../include/program.hpp',
    '../include/renderbuffer.hpp',
    '../include/rendercomponent.hpp',
    '../include/screen.hpp',
    '../include/shader.hpp',
    '../include/sizable.hpp',
    '../include/storage.hpp',
    '../include/texture.hpp',
    '../include/vertex_attrib.hpp',
    '../include/vertex_attrib.tpp',
    '../include/vertex_index.hpp',
    '../include/vgl.hpp',
)

sdk_src = files(
    'common.hpp',
    'context.cpp',
    'files.cpp',
    'framebuffer.cpp',
    'matrices.cpp',
    'program.cpp',
    'screen.cpp',
    'shader.cpp',
    'texture.cpp',
    'vertex_index.cpp',
)

add_project_arguments('-DGL_GLEXT_PROTOTYPES', language : ['c', 'cpp'])
add_project_link_arguments('-ljpeg', language : ['c', 'cpp'])

botson_sdk = shared_library(
    'botson_sdk',
    sdk_src,
    include_directories : [inc_src],
    dependencies : [dep_gl, dep_egl, dep_x11, dep_jpg, dep_png],
    install : true,
    soversion : '0.1.0',
)

install_headers(sdk_headers, subdir : 'SDK')

