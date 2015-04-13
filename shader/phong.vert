#version 410
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 ambient;
layout(location = 3) in vec3 diffuse;
layout(location = 4) in vec3 specular;
layout(location = 5) in float shininess;
layout(location = 6) in vec3 facenormal;
layout(location = 7) in vec2 texCoord;

out vec2 TexCoord;

struct LightProperties {
    bool isEnable;
    vec4 pos;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform LightProperties light;
uniform LightProperties dynamiclight;
uniform vec4 basicColor;
uniform int shadermode;
out vec4 globalColor;
out vec3 vNormal;
out Material material;
out vec3 eyeDir;
out vec3 lightDir;
out vec3 dynamiclightDir;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;
uniform mat4 normalMatrix;

void main()
{
    gl_Position = cameraToView * worldToCamera * modelToWorld*vec4(position, 1.0);
    TexCoord = texCoord;
    globalColor = basicColor;
    if(shadermode==1)
        vNormal = (normalMatrix*vec4(normal,0)).xyz;
    else
        vNormal = (normalMatrix*vec4(facenormal,0)).xyz;
    //vNormal = (transpose(inverse(worldToCamera * modelToWorld))*vec4(normal,0)).xyz;
    //vNormal = normal;
    material.ambient = ambient;
    material.diffuse = diffuse;
    material.specular = specular;
    material.shininess = shininess;

    vec3 vertexInCamSpace = (worldToCamera *modelToWorld*vec4(position, 1.0)).xyz;
    eyeDir = -vertexInCamSpace;
    vec3 dynamiclightInCamSpace = (worldToCamera*dynamiclight.pos).xyz;
    dynamiclightDir = dynamiclightInCamSpace - vertexInCamSpace;
    //lightDir = 0-lightInCamSpace;
    lightDir = 0-(light.pos).xyz;
    //lightDir = vec3(0,1,1);

}
