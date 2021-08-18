project('sis', 'cpp',
    default_options: 'cpp_std=c++11',
    version: '1.1.0',
    meson_versoin: '>= 0.42.0'
)

cpp = meson.get_compiler('cpp')

add_project_arguments('-DGL_GLEXT_PROTOTYPES', language : ['c', 'cpp'])
add_project_link_arguments('-ljpeg', language : ['c', 'cpp'])

# Dependencies
gl_dep = dependency('GL', required : true)
x11_dep = dependency('x11', required : false)
egl_dep = dependency('egl', required : false)
jpeg_dep = dependency('libjpeg', required : false)
png_dep = dependency('libpng', required : true)

sdk_priv_libs = []
if gl_dep.found()
    sdk_priv_libs += '-lGL'
endif
if x11_dep.found()
    sdk_priv_libs += '-lX11'
endif
if egl_dep.found()
    sdk_priv_libs += '-lEGL'
endif
if jpeg_dep.found()
    sdk_priv_libs += '-ljpeg'
endif
if png_dep.found()
    sdk_priv_libs += '-lpng'
endif

pkg = import('pkgconfig')

inc = include_directories('include')

subdir('include')
subdir('sis')
subdir('samples')

# Docs
doxygen = find_program('doxygen', required : false)
if doxygen.found()
    subdir('doc')
endif
