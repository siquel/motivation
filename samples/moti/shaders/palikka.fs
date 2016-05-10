#version 440
in vec3 v_pos;
in vec2 v_texCoord0;
in vec3 v_normal;

out vec4 color;
uniform sampler2D u_texture;
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
    vec3 lightColor = vec3(1.f, 1.f, 1.f);
    vec3 lightPos = vec3(u_view * u_lightPos);
    
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
    
    // diff
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(lightPos - v_pos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    // viewer is @ 0,0,0
    vec3 viewDir = normalize(-v_pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec4 obj = texture(u_texture, v_texCoord0);
    //vec4 obj = vec4(0.1, 0.2, 0.3, 1.0);
    vec4 result = vec4((ambient + diffuse + specular), 1.0) * obj;
    color =  result;
}