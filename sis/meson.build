sis_srcs = [
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
]

sis_lib = shared_library(
    'sis',
    sis_srcs,
    include_directories : inc,
    dependencies : [gl_dep, egl_dep, x11_dep, jpeg_dep, png_dep],
    install : true,
)

sis_dep = declare_dependency(link_with: sis_lib)

pkg.generate(
    name : 'sis',
    description : 'Sietium SDK',
    version : meson.project_version(),
    libraries : sis_lib,
    libraries_private : sdk_priv_libs
)
