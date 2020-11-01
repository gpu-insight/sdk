project(
    'Sietium SDK',
    ['c', 'cpp'],
    version : '1.1.0',
    default_options : ['c_std=c99', 'cpp_std=c++11']
)

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

# Dependencies
dep_gl = dependency('GL', required : true)
dep_x11 = dependency('x11', required : false)
dep_egl = dependency('egl', required : false)
dep_jpg = dependency('libjpeg', required : false)
dep_png = dependency('libpng', required : true)

sdk_priv_libs = []
if dep_gl.found()
    sdk_priv_libs += '-lGL'
endif
if dep_x11.found()
    sdk_priv_libs += '-lX11'
endif
if dep_egl.found()
    sdk_priv_libs += '-lEGL'
endif
if dep_jpg.found()
    sdk_priv_libs += '-ljpeg'
endif
if dep_jpg.found()
    sdk_priv_libs += '-lpng'
endif


pkg = import('pkgconfig')

subdir('sisdk')

# Docs
doxygen = find_program('doxygen', required : false)
if doxygen.found()
    subdir('doc')
endif

