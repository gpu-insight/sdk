# GPU-Insight SDK
The SDK encapsulates the manipulation of OpenGL texture, matrix, shaders and so on
in C++ so that it is easier to create an OpenGL application for developers.

## Build
```bash
meson build
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
