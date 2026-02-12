#version 330 core

in vec4 channelCol;
in vec2 channelTex;

out vec4 outCol;
in vec3 vWorldPos;

uniform bool uTreasureLightOn;
uniform vec3 uTreasureLightPos;

uniform sampler2D uTex;
uniform bool useTex;
uniform bool transparent;

void main()
{
	if (!useTex) {
		outCol = channelCol;
	}
	else {
		outCol = texture(uTex, channelTex);
		if (!transparent && outCol.a < 1) {
			outCol = vec4(1.0, 1.0, 1.0, 1.0);
		}
	}
	if (uTreasureLightOn)
	{
    float d = length(uTreasureLightPos - vWorldPos);
    float glow = 1.5 / (1.0 + 2.0*d*d);
    outCol.rgb += vec3(1.0, 0.6, 0.3) * glow;
	}	
}