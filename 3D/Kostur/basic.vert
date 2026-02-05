#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inCol;
layout(location = 2) in vec2 inTex;

uniform mat4 uM; // Model
uniform mat4 uV; // View
uniform mat4 uP; // Projection

out vec4 channelCol;
out vec2 channelTex;

void main()
{
    gl_Position = uP * uV * uM * vec4(inPos, 1.0);
    channelCol = inCol;
    channelTex = inTex;
}