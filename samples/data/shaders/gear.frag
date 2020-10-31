#version 330
precision mediump float;
in vec4 Color;
layout (location = 0) out vec4 fg_FragColor;

void main(void)
{
    fg_FragColor = Color;
}
