#version 440
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord0;
layout(location = 3) in vec2 a_texCoord1;
layout(location = 4) in vec2 a_texCoord2;
layout(location = 5) in vec2 a_texCoord3;
layout(location = 6) in vec2 a_texCoord4;
layout(location = 7) in vec2 a_texCoord5;
layout(location = 8) in vec2 a_texCoord6;
layout(location = 9) in vec2 a_texCoord7;

out vec3 v_pos;
out vec3 v_normal;
out vec2 v_texCoord0;

uniform vec4 u_lightPos;
uniform float u_time;

uniform vec4 u_viewRect;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_viewProj;
uniform mat4 u_model;
uniform mat4 u_modelViewProj;


void main()
{
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
    v_pos = vec3(u_model * vec4(a_position, 1.0));
    v_normal = mat3(transpose(inverse(u_model))) * a_normal;
    v_texCoord0 = a_texCoord0;
}
    
