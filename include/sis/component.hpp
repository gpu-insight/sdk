#pragma once
#include <GL/gl.h>
#include <GL/glut.h>
#include <map>
#include <vector>
#include "event.hpp"

using namespace std;

using Identifier = int;
class Component {
    Identifier  id;
protected:
    explicit Component() {
        id = rand()  % 10000 + 1;
    }

public:
    virtual void on(Event* e) = 0;
};
