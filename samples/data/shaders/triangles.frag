#version 430 core
#if 1
//#version 430 core

out vec4 fColor;
uniform float red;
uniform float green;
uniform float blue;
void main()
{
    fColor[0] = red;
    fColor[1] = green;
    fColor[2] = blue;
    fColor[3] = 1.0;
}
#else
//#version 130
uniform float red = 1.0;
uniform float green = 0.0;
uniform float blue = 0.0;
out vec4 fColor;
void main()
{
    fColor[0] = red;
    fColor[1] = green;
    fColor[2] = blue;
    fColor[3] = 1.0;
}
#endif
