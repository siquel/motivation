#version 440

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord0;
uniform float u_time;
uniform vec4 u_viewRect;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_viewProj;
uniform mat4 u_model;
uniform mat4 u_modelViewProj;

out vec3 v_pos;
out vec3 v_view;
out vec3 v_normal;
out vec4 v_color;
void main() {
    vec3 pos = a_position;

    float sx = sin(pos.x*32.0 + u_time.x*4.0)*0.5 + 0.5;
    float cy = cos(pos.y*32.0 + u_time.x*4.0)*0.5 + 0.5;
    vec3 displacement = vec3(sx, cy, sx*cy);
    vec3 normal = a_normal.xyz*2.0 - 1.0;

    pos = pos + normal*displacement*vec3(0.06, 0.06, 0.06);

    gl_Position = u_modelViewProj * vec4(pos, 1.0);

    mat4 mv = u_view * u_model;
    v_pos = gl_Position.xyz;
    v_view = (mv * vec4(pos, 1.0)).xyz;

    v_normal = (mv * vec4(normal, 0.0)).xyz;
    
    float len = length(displacement)*0.4 + 0.6;
    v_color = vec4(len, len, len, 1.0);
}