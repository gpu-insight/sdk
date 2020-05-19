import os

vars = Variables(None, ARGUMENTS)
vars.Add(PathVariable('INSTALL_DIR', 'Path to install', '/usr/local/botson', PathVariable.PathIsDir))

env = Environment(variables=vars,
                  CCFLAGS=['-fPIC', '-std=c++11', '-g', '-DGL_GLEXT_PROTOTYPES'])

env['lib_install'] = os.path.join("$INSTALL_DIR", "lib64")
env['inc_install'] = os.path.join("$INSTALL_DIR", "include/SDK")

def install_files(env, name, source, dest, *args, **kw):
    # source may be a non-iterable
    try:
        iter(source)
    except TypeError, te:
        source = [source]

    installed_files = []
    for src in source:
        installed_files += env.Install(env[dest], src)

    env.Alias(name, installed_files)

env.AddMethod(install_files, 'InstallFiles')

env.InstallFiles('install', Glob('include/*.hpp'), 'inc_install')

Export( 'env' )

# Subdirectory
SConscript( 'src/SConscript', variant_dir='build', duplicate=0 )

# Uninstall
env.Command("uninstall", None, Delete(FindInstalledFiles()))

