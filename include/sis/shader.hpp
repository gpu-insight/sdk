#pragma once

#include <GL/gl.h>
#include <cstdio>
#include <iostream>
#include <string>

class Shader {
public:
  static Shader file(GLenum type, std::string path);
  static Shader source(GLenum type, std::string src);
  unsigned int id() { return _id; }

private:
  GLuint _id;
  GLenum _type;
  Shader(GLuint id, GLenum type);
};

