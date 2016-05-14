#version 440
in vec3 v_pos;
in vec2 v_texCoord0;
in vec3 v_normal;
out vec4 color;

uniform sampler2D u_specularTexture;
uniform sampler2D u_diffuseTexture;
uniform sampler2D u_emissionTexture;

uniform vec4 u_lightPos;

uniform vec3 u_lightAmbient;
uniform vec3 u_lightDiffuse;
uniform vec3 u_lightSpecular;

uniform vec3 u_constantLinearQuadratic;
#define u_constant u_constantLinearQuadratic.x
#define u_linear u_constantLinearQuadratic.y
#define u_quadratic u_constantLinearQuadratic.z
#define u_shininess 32
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
    vec3 lightDir = vec3(0.0);
    
    if (u_lightPos.w == 0.0) {
        lightDir = normalize(-u_lightPos.xyz);
    } else {
        vec3 lightPos = vec3(u_view * u_lightPos);
        lightDir = normalize(lightPos - v_pos);
    }
    
    vec3 ambient = u_lightAmbient * vec3(texture(u_diffuseTexture, v_texCoord0));
    
    // diff
    vec3 normal = normalize(v_normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = u_lightDiffuse * diff * vec3(texture(u_diffuseTexture, v_texCoord0));
    
    // viewer is @ 0,0,0
    vec3 viewDir = normalize(-v_pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = u_lightSpecular * spec * vec3(texture(u_specularTexture, v_texCoord0));
    
    vec3 emission = texture(u_emissionTexture, v_texCoord0).xyz;
    
    // attenuation
    float distance = length(vec3(u_lightPos) - v_pos);
    float attenuation = 1.0 / (u_constant + u_linear * distance + u_quadratic * (distance * distance));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    vec4 result = vec4((ambient + diffuse + specular + emission), 1.0);
    color =  result;
}