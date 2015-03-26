#version 410
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightProperties {
    bool isEnable;
    vec4 pos;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform LightProperties light;
uniform LightProperties dynamiclight;
in vec4 globalColor;
in vec3 vNormal;
in vec3 eyeDir;
in vec3 lightDir;
in vec3 dynamiclightDir;
out vec4 fColor;
in Material material;

void main()
{
    //fColor = 0.1*lightColor*vColor*vec4(material.ambient,1.0f);
    //fColor = vec4(0.5,0.5,0.5,1);
    vec3 E = normalize(eyeDir);
    vec3 N = normalize(vNormal);

    vec3 ambientTerm = vec3(0);
    vec3 diffuseTerm = vec3(0);
    vec3 specularTerm = vec3(0);
    vec3 L, H;

    //static light
    L = normalize(lightDir);
    H = normalize(E + L);

    float diffuse = max(dot(L, N), 0);
    float specular = max(dot(H, N), 0);
    if (diffuse == 0.0)
        specular = 0.0;
    else
        specular = pow(specular, material.shininess);
    if(light.isEnable)
    {
        ambientTerm += (light.ambient).rgb;
        diffuseTerm += (light.diffuse).rgb*diffuse;
        specularTerm += (light.specular).rgb*specular;
    }
    //dynamiclight
    L = normalize(dynamiclightDir);
    H = normalize(E + L);
    diffuse = max(dot(L, N), 0);
    specular = max(dot(H, N), 0);
    if (diffuse == 0.0)
        specular = 0.0;
    else
        specular = pow(specular, material.shininess);
    if(dynamiclight.isEnable)
    {
        diffuseTerm += (dynamiclight.diffuse).rgb*diffuse;
        specularTerm += (dynamiclight.specular).rgb*specular;
        ambientTerm += (dynamiclight.ambient).rgb;
    }
    //material
    ambientTerm *= material.ambient;
    diffuseTerm *= material.diffuse;
    specularTerm *= material.specular;

    fColor = 0.2*globalColor+vec4(min(ambientTerm + diffuseTerm + specularTerm,vec3(1.0)), 1.0);
}
