#include <sis/program.hpp>

void Program::activate() { glUseProgram(program); }

unsigned int Program::attribute(const std::string &name) {
    return glGetAttribLocation(program, name.c_str());
}

void Program::attribute(const std::string &name, float value) {
    GLint location = glGetAttribLocation(program, name.c_str());
    glVertexAttrib1f(location, value);
}

void Program::uniform(const std::string &name, float value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniform1f(location, value);
}

void Program::uniform(const std::string &name, int value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    glUniform1i(location, value);
}

unsigned int Program::uniform(const std::string &name) {
    return glGetUniformLocation(program, name.c_str());
}

using StdPair = std::pair<GLenum, std::string>;
using ShaderConfig = std::vector<std::pair<GLenum, std::string>>;

Program::Program(std::vector<Shader> shaders) {
    program = glCreateProgram();

    for (auto s : shaders) {
        glAttachShader(program, s.id());
    }

    GLint linked;

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint length;
        GLchar *log;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        log = (GLchar *)malloc(length);
        glGetProgramInfoLog(program, length, &length, log);
        std::cout << log << std::endl;
        free(log);
        glDeleteProgram(program);
        exit(EXIT_FAILURE);
    }
}

void Program::feedback(const std::vector<std::string> &varyings, GLenum mode) {
    auto **vs = (GLchar **)malloc(varyings.size() * sizeof(GLchar *));
    for (auto i = 0; i < varyings.size(); i++) {
        vs[i] = (GLchar *)varyings[i].c_str();
    }
    //  const char* vs[] = {
    //      "foo"
    //  };
    glTransformFeedbackVaryings(program, sizeof(vs) / sizeof(vs[0]), vs, mode);
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint length;
        GLchar *log;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        log = (GLchar *)malloc(length);
        glGetProgramInfoLog(program, length, &length, log);
        std::cout << log << std::endl;
        free(log);
        glDeleteProgram(program);
        exit(EXIT_FAILURE);
    }
}

void Program::uniform(const std::string &name, glm::mat4 value) {
    GLint location = glGetUniformLocation(program, name.c_str());
    const float *v = glm::value_ptr(value);
    glUniformMatrix4fv(location, 1, false, v);
}
