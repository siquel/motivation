#version 440
in vec3 v_pos;
in vec2 v_texCoord0;
in vec3 v_normal;
out vec4 color;

uniform sampler2D u_texture;
uniform sampler2D u_texture2;

uniform vec3 u_lightPos;

uniform vec3 u_lightAmbient;
uniform vec3 u_lightDiffuse;
uniform vec3 u_lightSpecular;

uniform float u_time;
uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;


//////////// predef
uniform vec4 u_viewRect;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_viewProj;
uniform mat4 u_model;
uniform mat4 u_modelViewProj;
/////////////

void main()
{
    vec3 lightPos = vec3(u_view * vec4(u_lightPos, 1.0));
    
    vec3 ambient = u_lightAmbient * u_ambient;
    
    // diff
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(lightPos - v_pos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = u_lightDiffuse * (diff * u_diffuse);
    
    // viewer is @ 0,0,0
    vec3 viewDir = normalize(-v_pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = u_lightSpecular * (spec * u_specular);
    
    vec4 obj = mix(
        texture(u_texture, v_texCoord0),
        texture(u_texture2, v_texCoord0),
        0.2);
    //vec4 obj = vec4(0.1, 0.2, 0.3, 1.0);
    //vec4 result = vec4((ambient + diffuse + specular), 1.0) * obj;
    vec4 result = vec4((ambient + diffuse + specular), 1.0);
    color =  result;
}