import os

env = Environment(CCFLAGS=['-std=c++11', '-g'],
                  CPPDEFINES={'GL_GLEXT_PROTOTYPES': ''})

sources = Glob('*.cpp')

sdk = env.SharedLibrary(target="btxsdk",
                        source=sources,
                        LIBS=['GL', 'glut', 'jpeg', 'png'],
                        SHLIBVERSION="1.1.1")

# env.InstallVersionedLib(target="lib", source=sdk)