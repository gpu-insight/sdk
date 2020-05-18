import os

prefix = "/usr/lib64/botson"

env = Environment(CCFLAGS=['-std=c++11', '-g', '-DGL_GLEXT_PROTOTYPES'])

sources = Glob('src/*.cpp')

btx_sdk = env.SharedLibrary(target="btxsdk",
                        source=sources,
                        LIBS=['GL', 'glut', 'jpeg', 'png'],
                        SHLIBVERSION="0.0.1")

env.Alias("install", env.Install(os.path.join(prefix, "lib"), btx_sdk))
