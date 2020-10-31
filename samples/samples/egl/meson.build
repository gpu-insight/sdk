samples = [
    ['cube', ['cubemap_seamless.cpp']],
    ['depth', ['depth.cpp']],
]

foreach s : samples
    executable(
        s[0],
        s[1],
        dependencies : [gl_dep, sdk_dep, egl_dep],
        install : true,
        install_dir : get_option('bindir')
    )
endforeach
