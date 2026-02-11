#version 330 core
in vec4 channelCol;
in vec2 channelTex;
out vec4 outCol;

uniform sampler2D uTex;
uniform bool transparent;

void main() {
    vec4 texColor = texture(uTex, channelTex);
    outCol = texColor;
    if (transparent && texColor.a < 1.0) {
        outCol.a = texColor.a;
    }
}