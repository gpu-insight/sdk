#version 430

out vec2 texCoord;

vec2 calc_texcoord(vec2 pos)
{
    return (pos * 0.5f + 0.5f);
}

void main()
{
    vec2 p0 = vec2(-1.0, -1.0);
    vec2 p1 = vec2(-1.0,  1.0);
    vec2 p2 = vec2( 1.0, -1.0);
    vec2 p3 = vec2( 1.0,  1.0);

    switch (gl_VertexID)
    {
        case 0: 
        {
            texCoord = calc_texcoord(p0);
            gl_Position = vec4(p0.x, p0.y, 0.0, 1.0);
            break;
        }
        case 1: 
        {
            texCoord = calc_texcoord(p1);
            gl_Position = vec4(p1.x, p1.y, 0.0, 1.0);
            break;
        }
        case 2: 
        {
            texCoord = calc_texcoord(p2);
            gl_Position = vec4(p2.x, p2.y, 0.0, 1.0);
            break;
        }
        case 3: 
        {
            texCoord = calc_texcoord(p3);
            gl_Position = vec4(p3.x, p3.y, 0.0, 1.0);
            break;
        }
    }
}
