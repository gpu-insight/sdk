# Botson SDK
The SDK encapsulates the manipulation of OpenGL texture, matrix, shaders and so on
in C++ so that it is easier to create an OpenGL application for developers.

## Build
```bash
meson build
```

## Install
```bash
sudo ninja-build -C build install
```

By default libbotson_sdk is installed at system library directory(/usr/local/lib64) and its documents are installed at
`/usr/local/share/doc/botson-sdk`. If you intend to change the default installation directory, then run this before installation

```bash
meson configure -Dprefix=/path/to/your/intended/dir build
```
