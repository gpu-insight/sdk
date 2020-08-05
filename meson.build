project(
    'Botson SDK',
    ['c', 'cpp'],
    version : '0.1.0',
    default_options : ['c_std=c99', 'cpp_std=c++11']
)

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

# Dependencies
dep_gl = dependency('GL', required : true)
dep_x11 = dependency('x11', required : false)
dep_egl = dependency('egl', required : false)
dep_jpg = dependency('libjpeg', required : true)
dep_png = dependency('libpng', required : true)

# Headers
inc_src = include_directories('include')

subdir('src')