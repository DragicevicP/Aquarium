#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

out vec2 vUV;

uniform vec2 u_resolution;

void main()
{
    vec2 pos = vec2(
        (aPos.x / u_resolution.x) * 2.0 - 1.0,
        (aPos.y / u_resolution.y) * 2.0 - 1.0
    );

    pos.y = -pos.y; 

    gl_Position = vec4(pos, 0.0, 1.0);
    vUV = aUV;
}