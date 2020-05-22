import os

vars = Variables(None, ARGUMENTS)
vars.Add(PathVariable('INSTALL_DIR', 'Path to install', '/usr/local/botson', PathVariable.PathIsDir))

env = Environment(variables=vars,
                  CCFLAGS=['-fPIC', '-std=c++11', '-g', '-DGL_GLEXT_PROTOTYPES'],
                  CPPPATH=['$INSTALL_DIR/include', os.path.join(GetLaunchDir(), 'include')])


def install_libs(env, name, source, *args, **kw):
    # source may be a non-iterable
    try:
        iter(source)
    except TypeError as te:
        source = [source]

    installed_files = []
    for src in source:
        installed_files += env.InstallVersionedLib(target=os.path.join('$INSTALL_DIR', 'lib64'), source=src)

    env.Alias(name, installed_files)


env.AddMethod(install_libs, 'InstallLibs')

env.Alias("install", env.Install(os.path.join('$INSTALL_DIR', 'include/SDK'), Glob('include/*.[ht]pp')))

Export('env')

# Subdirectory
SConscript('src/SConscript', variant_dir='build', duplicate=0)

# Uninstall
env.Command("uninstall", None, Delete(FindInstalledFiles()))

