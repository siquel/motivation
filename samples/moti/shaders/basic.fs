#version 440
out vec4 color;

uniform vec4 u_viewRect;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_viewProj;
uniform mat4 u_model;
uniform mat4 u_modelViewProj;

void main()
{    
    color =  vec4(1.0, 1.0, 1.0, 1.0);
}