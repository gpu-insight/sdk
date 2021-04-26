#pragma once

#include "component.hpp"

class RenderComponent : public Component {
public:
    virtual void init() = 0;
    virtual void render() = 0;
};

template <typename T>
using Scene = std::vector<T*>;
