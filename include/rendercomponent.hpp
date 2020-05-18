#pragma once

#include "component.hpp"
#include "botson_sdk.hpp"

BOTSON_SDK_API
class RenderComponent : public Component {
public:
    virtual void init() = 0;
    virtual void render() = 0;
};

BOTSON_SDK_API
template <typename T>
using Scene = std::vector<T*>;
