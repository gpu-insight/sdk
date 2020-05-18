import os

prefix = "/usr/lib64/botson"
VariantDir('build', 'src', duplicate=0)

env = Environment(CCFLAGS=['-std=c++11', '-g', '-DGL_GLEXT_PROTOTYPES'])

btx_sdk = env.SharedLibrary(target="btxsdk",
                        source=Glob('build/*.cpp'),
                        LIBS=['GL', 'glut', 'jpeg', 'png'],
                        SHLIBVERSION="0.0.1")

env.Alias("install", env.Install(os.path.join(prefix, "lib"), btx_sdk))
