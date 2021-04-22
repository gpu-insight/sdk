GPUInsight® SDK & Samples
=========================

[GPUInsight](https://www.gpuinsight.com/) SDK provides the abstraction of drawing, texture and shader program operations in OpenGL API. Besides it also simplifies the presentation of 3D rendering and creation of OpenGL demo.

# Prerequisites
The following devel packages are required to compile this library.
- libEGL
- libGL
- libX11
- jpeg
- png
- glm
- cmake 3.9

# Build & Install
## CMake
```
cmake -S . -B build
cmake --build build
cmake --install build
```

## Meson
```bash
meson build
meson compile -C build
meson install -C build
```

