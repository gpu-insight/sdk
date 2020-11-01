
#include "../include/matrices.hpp"

Matrices *Matrices::_instance = nullptr;

Matrices *Matrices::instance() {
    if (_instance == nullptr) {
        _instance = new Matrices();
    }

    return _instance;
}

void Matrices::rotate(float angle, float x, float y, float z) {
    this->_model = glm::rotate(this->_model, angle, glm::vec3(x, y, z));
}

void Matrices::translate(float x, float y, float z) {
    this->_model = glm::translate(this->_model, glm::vec3(x, y, z));
}

void Matrices::scale(float x, float y, float z) {
    this->_model = glm::scale(this->_model, glm::vec3(x, y, z));
}

void Matrices::lookat(float ex, float ey, float ez,
                      float cx, float cy, float cz, float ux, float uy, float uz) {
    this->_camera = glm::vec3(ex, ey, ez);
    this->_view = glm::lookAt(glm::vec3(ex, ey, ez), glm::vec3(cx, cy, cz), glm::vec3(ux, uy, uz));

}

void Matrices::lookat(glm::vec3 eye, glm::vec3 center, glm::vec3 upper) {
    this->_camera = eye;
    this->_view = glm::lookAt(eye, center, upper);

}

void Matrices::frustum(float left, float right, float bottom, float top, float near, float far) {
    this->_projection = glm::frustum(left, right, bottom, top, near, far);
}

void Matrices::perspective(float fovy, float aspect, float near, float far) {
    this->_projection = glm::perspective(fovy, aspect, near, far);
}

void Matrices::push() {
    this->_matrixes.push(_model);
}

void Matrices::pop() {
    this->_model = this->_matrixes.top();
    this->_matrixes.pop();
}

void Matrices::identity() {
    this->_model = glm::mat4(1.0f);
}

glm::mat4 Matrices::mvp() {
    return _projection * _view * _model;
}

glm::mat4 Matrices::vp() {
    return _projection * _view;
}

glm::mat4 Matrices::view() {
    return this->_view;
}

glm::mat4 Matrices::model() { return this->_model; }

glm::vec3 Matrices::camera() {
    return this->_camera;
}

void Matrices::ortho(float negative_x, float positive_x, float negative_y, float positive_y) {
    this->_projection = glm::ortho(negative_x, positive_x, negative_y, positive_y, -512.0f, 512.0f);
}
