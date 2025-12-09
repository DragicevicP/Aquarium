#version 330 core

layout(location = 0) in vec2 aPos; 
uniform vec2 u_resolution; 

void main()
{   
    vec2 zeroToOne = aPos / u_resolution;
    vec2 clip = zeroToOne * 2.0 - 1.0;
    
    clip.y = -clip.y;
    gl_Position = vec4(clip, 0.0, 1.0);
}