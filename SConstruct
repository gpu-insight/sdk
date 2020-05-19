import os

VariantDir('build', 'src', duplicate=0)

vars = Variables(None, ARGUMENTS)
vars.Add(PathVariable('INSTALL_DIR', 'Path to install', '/usr/local/botson', PathVariable.PathIsDir))


env = Environment(variables=vars, CCFLAGS=['-fPIC', '-std=c++11', '-g', '-DGL_GLEXT_PROTOTYPES'])

btx_sdk = env.SharedLibrary(target="btxsdk",
                        source=Glob('build/*.cpp'),
                        LIBS=['GL', 'glut', 'jpeg', 'png'],
                        SHLIBVERSION="0.0.1")

env.Alias("install", env.Install(os.path.join("$INSTALL_DIR", "lib64"), btx_sdk))
env.Alias("install", env.Install(os.path.join("$INSTALL_DIR", "include/SDK"), Glob('include/*.hpp')))

