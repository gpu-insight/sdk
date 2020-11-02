Sietium® SDK & Samples
======================

The SDK provides the abstraction of drawing, texture and shader program operations in OpenGL API. Besides it also simplifies the presentation of 3D rendering and creation of OpenGL demo.

# Prerequisites
The following devel packages are required to compile this library.
- EGL (optional)
- freeglut3 (optional)
- glm
- GL
- jpeg
- png
- X11

# Build & Install
This project supports both of build systems, familiar CMake or faster [Meson](https://mesonbuild.com/). It is noted that the SDK library and samples are put together but the latter depends on the former. Consequently you'd better build the library before samples.

## CMake
```
mkdir build && cd build
cmake ..
make && make install        # build SDK

cd samples
make && make install        # build Samples
```

## Meson
```bash
meson build
meson compile -C build
meson install -C build
```

NOTE: The library will be installed at system directory and so will its docs. If you intend to change the
default installation directory, then run this before installation

```bash
meson --prefix=/path/to/your/directory build
```
