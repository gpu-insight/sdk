#pragma once

#include <iostream>
#include <GLFW/glfw3.h>
#include "botson_sdk.hpp"
#include "component.hpp"
#include "matrices.hpp"

BOTSON_SDK_API
class Camera : public Component {
public:
    void on(Event *event) override;

    Camera(
        int eyeX = 0.0, int eyeY = 0.0, int eyeZ = 0.0,
        float fovy = 64.0, float aspect = 1.0, float near = 1.0, float far = 500.0
    );

private:
    int ex, ey, ez;
    int cx, cy, cz;
    int ux, uy, uz;
    float fovy, aspect, near, far;
};

Camera::Camera(
int eyeX, int eyeY, int eyeZ,
float fovy, float aspect, float near, float far
) {

    this->ex = eyeX;
    this->ey = eyeY;
    this->ez = eyeZ;

    this->cx = 0;
    this->cy = 0;
    this->cz = -1.0;

    this->ux = 0;
    this->uy = 1;
    this->uz = 0;

    this->aspect = aspect;
    this->near = near;
    this->far = far;
    this->fovy = fovy;

    Matrices *instance = Matrices::instance();
    instance->lookat(ex, ey, ez, cx, cy, cz, ux, uy, uz);
    instance->perspective(glm::radians(fovy), aspect, near, far);
}


void Camera::on(Event *event) {
    auto change = [&](int x) -> int {
        if (glm::sqrt(cx * cx + cy * cy + cz * cz) >= 1500) {
            return 0;
        } else {
            return x;
        }
    };
    if (auto r = dynamic_cast<KeyEvent *>(event)) {
        switch (r->key()) {
            case GLFW_KEY_A:
                cx -= change(2);
                break;
            case GLFW_KEY_D:
                cx += change(2);
                break;
            case GLFW_KEY_W:
                cz -= change(2);
                break;
            case GLFW_KEY_S:
                cz += change(2);
                break;
            case GLFW_KEY_Z:
                cy -= change(1);
                break;
            case GLFW_KEY_X:
                cy += change(1);
                break;
        }
    }

    if (auto r = dynamic_cast<MouseEvent *>(event)) {
        switch (r->button()) {
            case GLFW_MOUSE_BUTTON_LEFT:
//                cy -= change(2);
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
//                cy += change(2);
                break;
        }
    }

    std::cout << " " << cx << " " << cy << " " << cz << std::endl;

    Matrices *instance = Matrices::instance();
    instance->lookat(ex, ey, ez, cx, cy, cz, ux, uy, uz);
    instance->perspective(glm::radians(fovy), aspect, near, far);
}
