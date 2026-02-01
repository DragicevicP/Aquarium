#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

out vec2 vUV;

uniform vec2 u_resolution;

void main()
{
    vec2 zeroToOne = aPos / u_resolution;
    vec2 zeroToTwo = zeroToOne * 2.0;
    vec2 clip = zeroToTwo - 1.0;

    gl_Position = vec4(clip * vec2(1.0, -1.0), 0.0, 1.0);
    vUV = aUV;
}