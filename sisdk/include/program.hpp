#pragma once

#include "shader.hpp"
#include <GL/gl.h>
#include <iostream>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "botson_sdk.hpp"

BOTSON_SDK_API
class Program {
public:
  template <typename... Args>
  static Program *from(Shader shader, Args... args) {
    const int size = sizeof...(args) + 1;
    Shader shaders[size] = {shader, args...};
    std::vector<Shader> vshaders;
    for (int i = 0; i < size; i++) {
      vshaders.push_back(shaders[i]);
    }
    return new Program(vshaders);
  }

  void activate();

  unsigned int attribute(const std::string &name);

  unsigned int uniform(const std::string &name);

  void attribute(const std::string &name, float value);

  void uniform(const std::string &name, float value);

  void uniform(const std::string &name, int value);

  void uniform(const std::string &name, glm::mat4 value);

  void feedback(const std::vector<std::string> &varyings, GLenum mode);

private:
  GLuint shader, program;

  Program(std::vector<Shader>);
};

