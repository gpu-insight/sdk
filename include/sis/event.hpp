#pragma once

class Event {
    virtual void type() {};
};

class MouseEvent : public Event {

public:
    MouseEvent(int button, int state, float x, float y): _button(button), _state(state), _x(x), _y(y) {}

    int button() { return _button; }
    float x() { return _x; };
    float y() { return _y; };
private:
    int _button, _state;
    float _x, _y;
};

class KeyEvent : public Event {
public:
    KeyEvent(unsigned char key, float x, float y) : _key(key), _x(x), _y(y) {}

    unsigned char key() {return _key;};

    float x() { return _x; };

    float y() { return _y; };

private:
    unsigned char _key;
    float _x;
    float _y;
};

class ResizeEvent : public Event {
public:
    ResizeEvent(int w, int h): _w(w), _h(h) {    }
private:
    int _w, _h;
};

