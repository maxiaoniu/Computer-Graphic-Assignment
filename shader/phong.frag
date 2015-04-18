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
uniform sampler2D orange;
uniform sampler2D circle;
uniform sampler2D wall;
uniform sampler2D wallNormal;
uniform int bumpSel;
uniform int textureSel;
uniform LightProperties light;
uniform LightProperties dynamiclight;
in vec4 globalColor;
in vec3 vNormal;
in vec3 eyeDir;
in vec3 lightDir;
in vec3 dynamiclightDir;
out vec4 fColor;
in Material material;
in vec2 TexCoord;
void main()
{
    //fColor = 0.1*lightColor*vColor*vec4(material.ambient,1.0f);
    //fColor = vec4(0.5,0.5,0.5,1);
    vec3 E = normalize(eyeDir);
    //vec3 N = normalize(vNormal);
    vec3 N ;
    if(bumpSel==0)
        N = normalize(texture(circle, TexCoord).rgb*2.0-1.0);
    else if(bumpSel==1)
        N = normalize(texture(orange, TexCoord).rgb*2.0-1.0);
    else if(bumpSel==2)
        N = normalize(vNormal);
    else if(bumpSel==3)
        N = normalize(texture(wallNormal, TexCoord).rgb*2.0-1.0);
    vec3 ambientTerm = vec3(0);
    vec3 diffuseTerm = vec3(0);
    vec3 specularTerm = vec3(0);
    vec3 L, H;

    //static light
    L = normalize(lightDir);
    H = normalize(E + L);

    float diffuse = max(dot(L, N), 0);
    float specular = max(dot(H, N), 0);

    if (diffuse == 0.0 || material.shininess==0)
        specular = 0.0;
    else
        specular = pow(specular, material.shininess);
    if(light.isEnable)
    {
        ambientTerm += (light.ambient).rgb;
        //add texture wall
        diffuseTerm += (light.diffuse).rgb*diffuse;
        //diffuseTerm += (light.diffuse).rgb*diffuse;

        specularTerm += (light.specular).rgb*specular;
    }
    //dynamiclight
    L = normalize(dynamiclightDir);
    H = normalize(E + L);
    diffuse = max(dot(L, N), 0);
    specular = max(dot(H, N), 0);
    if (diffuse == 0.0 || material.shininess==0)
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
    if(textureSel==1)
        fColor = vec4((0.2*globalColor.rgb+min(ambientTerm + diffuseTerm + specularTerm,vec3(1.0)))*texture(wall, TexCoord).rgb, 1.0);
    else
        fColor = vec4((0.2*globalColor.rgb+min(ambientTerm + diffuseTerm + specularTerm,vec3(1.0))), 1.0);
    //fColor = vec4(lightDir,1.0);
}
