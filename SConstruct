import os

AddOption('--prefix', dest='prefix', action='store', default='/usr', help='install prefix')

env = Environment(PREFIX=GetOption('prefix'),
                  CCFLAGS=['-fPIC', '-std=c++11', '-g', '-DGL_GLEXT_PROTOTYPES'],
                  CPPPATH=['#include'])


def install_libs(env, name, source, *args, **kw):
    # source may be a non-iterable
    try:
        iter(source)
    except TypeError as te:
        source = [source]

    installed_files = []
    for src in source:
        installed_files += env.InstallVersionedLib(target='$PREFIX/lib64', source=src)

    env.Alias(name, installed_files)


env.AddMethod(install_libs, 'InstallLibs')

env.Alias("install", env.Install('$PREFIX/include/SDK', Glob('include/*.[ht]pp')))

Export('env')

# Subdirectory
SConscript('src/SConscript', variant_dir='build', duplicate=0)

# Uninstall
env.Command("uninstall", None, Delete(FindInstalledFiles()))

