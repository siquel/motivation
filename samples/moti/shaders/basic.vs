#version 440
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord0;

uniform vec4 u_viewRect;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_viewProj;
uniform mat4 u_model;
uniform mat4 u_modelViewProj;


void main()
{
    vec4 pos = u_modelViewProj * vec4(a_position, 1.0);
    gl_Position = pos;
}
    