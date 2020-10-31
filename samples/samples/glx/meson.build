samples = [
    ['glxgears', ['glxgears.cpp', 'save2bmp.cpp']],
    ['ren2tex', ['render2tex.cpp', 'save2bmp.cpp']],
]

foreach s : samples
    executable(
        s[0],
        s[1],
        cpp_args : ['-Wno-narrowing','-DGLX_GLXEXT_PROTOTYPES'],
        dependencies : [gl_dep, sdk_dep, x11_dep],
        include_directories : incdir,
        install : true,
        install_dir : get_option('bindir')
    )
endforeach

