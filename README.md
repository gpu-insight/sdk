# GPU-Insight SDK
The SDK encapsulates the manipulation of OpenGL texture, matrix, shaders and so on
in C++ so that it is easier to create an OpenGL application for developers.

## Prerequisites
The following devel packages are required to compile this library.
- EGL (optional)
- freeglut3 (optional)
- glm
- GL
- jpeg
- png
- X11

## Compilation
```bash
meson build
ninja -C build
```

## Install
```bash
sudo ninja -C build install
```

NOTE: The library will be installed at system directory and so will its docs. If you intend to change the
default installation directory, then run this before installation

```bash
meson configure -Dprefix=/path/to/your/directory build
```
