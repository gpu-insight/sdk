inc_samples = include_directories('.')

conf = configuration_data()
conf.set_quoted('SAMPLES_DATA_PATH', join_paths(get_option('prefix'), get_option('datadir'), 'sietium'))
configure_file(
    output : 'config.h',
    configuration : conf
)

subdir('data')

# Specific dependencies
x11_dep = dependency('x11', required : false)
if x11_dep.found()
    subdir('glxgears')
endif
