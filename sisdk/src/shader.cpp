#include "../include/shader.hpp"
#include "../include/files.hpp"
#include <stdexcept>

Shader Shader::file(GLenum type, std::string path) {
    std::string s(text(path));
    return source(type, s);
}

Shader Shader::source(GLenum type, std::string src) {
    GLuint sh = glCreateShader(type);
    const GLchar *str = src.c_str();
    glShaderSource(sh, 1, &str, NULL);
    glCompileShader(sh);
    GLint compiled;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint length;
        GLchar *log;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &length);

        log = (GLchar *)malloc(length);
        glGetShaderInfoLog(sh, length, &length, log);
        glDeleteShader(sh);
        std::string err(log);
        free(log);
        throw std::invalid_argument(err);
    }

    return Shader(sh, type);
}

Shader::Shader(GLuint id, GLenum type) {
    this->_id = id;
    this->_type = type;
}
