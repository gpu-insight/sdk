#version 430 core
#if 10
//#version 430 core
layout(location = 0) in vec4 vPosition;

uniform float x_offset;
uniform float y_offset;
uniform float z_offset;
mat4 transform = mat4(1.0, 0.0, 0.0, 0.0, 
		  0.0, 1.0, 0.0, 0.0,
		  0.0, 0.0, 1.0, 0.0,
		  0.0, 0.0, 0.0, 1.0);

void main()
{
	/*
    transform[3][0] = x_offset; 
    transform[3][1] = y_offset; 
    transform[3][2] = z_offset; 
    gl_Position = transform * vPosition;
    */
    gl_Position = vPosition;
    gl_Position.x = vPosition.x + x_offset;
}
#else
//#version 130
uniform float x_offset = 0.0;
uniform float y_offset = 0.0;
uniform float z_offset = 0.0;
mat4 transform = mat4(1.0, 0.0, 0.0, 0.0, 
		  0.0, 1.0, 0.0, 0.0,
		  0.0, 0.0, 1.0, 0.0,
		  0.0, 0.0, 0.0, 1.0);

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_Position.x += x_offset;
    gl_Position.y += y_offset;
    gl_Position.z += z_offset;
}
#endif
