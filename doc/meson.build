cdata = configuration_data()
cdata.set('VERSION', meson.project_version())
cdata.set('TOP_SRCDIR', meson.source_root())
cdata.set('TOP_BUILDDIR', meson.build_root())

doxyfile = configure_file(
    input : 'Doxyfile.in',
    output : 'Doxyfile',
    configuration : cdata,
    install : false
)

doc_target = custom_target(
    'botson-sdk-docs',
    input : doxyfile,
    output : 'html',
    command : [doxygen, doxyfile],
    install : true,
    install_dir : join_paths(get_option('datadir'), 'doc', 'botson-sdk')
)
