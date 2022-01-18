#pragma once

#include "shader.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/gl.h>

using std::string;

static inline string shader_source_from_string_or_file(const string& s)
{
    std::ifstream maybe_file(s);

    if (!maybe_file.is_open()) {
        // Input has seemingly been a shader source string
        return s;
    }

    return string((std::istreambuf_iterator<char>(maybe_file)),
                   std::istreambuf_iterator<char>());
}

template<GLenum ST>
class NShader {
public:
    NShader() {}

    template<typename... Ts>
    NShader(Ts... sources) {
        constexpr std::size_t n_sources = sizeof...(sources);
        std::vector<string> dummy = {
            shader_source_from_string_or_file(std::move(sources))...
        };

        std::array<const GLchar*, n_sources> p_sources;

        std::transform(dummy.cbegin(), dummy.cend(), p_sources.begin(),
            [](const string& s) -> const GLchar* { return s.c_str(); });

        _id = glCreateShader(ST);
        glShaderSource(_id, n_sources, p_sources.data(), NULL);
        glCompileShader(_id);
        GLint compiled;
        glGetShaderiv(_id, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint length;
            GLchar *log;
            glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &length);

            log = (GLchar *)malloc(length);
            glGetShaderInfoLog(_id, length, &length, log);
            glDeleteShader(_id);
            string err(log);
            free(log);
            throw std::invalid_argument(err);
        }
    };

    ~NShader() {
        glDeleteShader(_id);
    }

    GLuint id() { return _id; }

private:
    GLuint _id;
};

//
// never instantiated
//
template<typename... T>
class NProgram {

};

//
// zero argument specialization
//
// the root of the whole hierarchy of inheritance as well as
// the end of recursion of inheritance
//
template<>
class NProgram<> {
public:
    NProgram(): _id(glCreateProgram()) {}
    ~NProgram() { glDeleteProgram(_id); }

    const GLuint id() const { return _id; }

private:
    GLuint _id;
};

//
// one argument and one parameter pack specialization
//
template<typename First, typename... Rest>
class NProgram<First, Rest...> : public NProgram<Rest...> {
public:
    NProgram() {}

    NProgram(const First& v, const Rest&... vrest)
        : _first(v), NProgram<Rest...>(vrest...) {
        glAttachShader(this->id(), _first.id());
    }

    template<typename... Types>
    NProgram(const NProgram<Types...>& rhs)
        : _first(rhs.first()), NProgram<Rest...>(rhs.rest()) {}

    template<typename... Types>
    NProgram& operator=(const NProgram<Types...>& rhs) {
        _first = rhs.first();
        rest() = rhs.rest();
        return *this;
    }

    //
    // Helpers
    //
    First& first() { return _first; }
    const First& first() const {  return _first; }

    NProgram<Rest...>& rest() { return *this; }
    const NProgram<Rest...>& rest() const { return *this; }

    void activate() {
        // Deferred linkage
        glLinkProgram(this->id());

        // Activate program
        glUseProgram(this->id());
    }

    unsigned int uniform(const string& name) {
        return glGetUniformLocation(this->id(), name.c_str());
    }

protected:
    First _first;
};


// full specialization
template<int... T>
class NProgram<NShader<T>...> {
};

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

  unsigned int attribute(const string &name);

  unsigned int uniform(const string &name);

  void attribute(const string &name, float value);

  void uniform(const string &name, float value);

  void uniform(const string &name, int value);

  void uniform(const string &name, glm::mat4 value);

  void feedback(const std::vector<string> &varyings, GLenum mode);

private:
  GLuint shader, program;

  Program(std::vector<Shader>);
};

