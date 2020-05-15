#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <cstdlib>
#include <stack>

using namespace std;

class Matrices {
public:
    static Matrices *instance();

    void translate(float x, float y, float z);

    void scale(float x, float y, float z);

    void rotate(float angle, float x, float y, float z);

    void lookat(float ex, float ey, float ez, float cx, float cy, float cz, float ux, float uy, float uz);

    void frustum(float left, float right, float bottom, float top, float near, float far);

    void identity();

    void push();

    void pop();

    glm::mat4 mvp();

    glm::mat4 vp();

    glm::vec3 camera();

    void perspective(float fovy, float aspect, float near, float far);

    glm::mat4 view();

    glm::mat4 model();

    void lookat(glm::vec3 eye, glm::vec3 center, glm::vec3 upper);

    void ortho(float negative_x, float positive_x, float negative_y, float positive_y);

private:
    static Matrices *_instance;

    stack<glm::mat4> _matrixes;
    glm::mat4 _model = glm::mat4(1.0f);
    glm::mat4 _projection = glm::mat4(1.0f);

    glm::mat4 _view = glm::mat4(1.0f);

    Matrices() {

    }

    Matrices(Matrices &) {}

    Matrices &operator=(Matrices const &) {}

    glm::vec3 _camera;

};
