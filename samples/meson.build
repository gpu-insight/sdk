incdir = include_directories('.')

conf = configuration_data()
conf.set_quoted('SAMPLES_DATA_PATH', join_paths(get_option('prefix'), get_option('datadir'), 'sietium'))
configure_file(
    output : 'config.h',
    configuration : conf
)

subdir('data')

# General dependencies
gl_dep = dependency('gl', required : true)
sdk_dep = dependency('botson_sdk', required : true)

# Specific dependencies
glut_dep = dependency('glut', required : false)
if glut_dep.found()
    subdir('samples/glut')
endif

egl_dep = dependency('egl', required : false)
if egl_dep.found()
    subdir('samples/egl')
endif

x11_dep = dependency('x11', required : false)
if x11_dep.found()
    subdir('samples/glx')
endif
