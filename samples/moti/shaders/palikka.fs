#version 440
in vec2 v_texCoord0;
out vec4 color;
uniform sampler2D u_texture;
uniform float u_time;
uniform vec4 u_viewRect;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_viewProj;
uniform mat4 u_model;
uniform mat4 u_modelViewProj;
void main()
{
    vec2 uv = v_texCoord0;
    color = texture(u_texture, uv); 
}