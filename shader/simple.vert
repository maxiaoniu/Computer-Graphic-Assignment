#version 410
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
uniform vec4 basicColor;
//out vec4 vColor;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

out Data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    mat4 mvp;
} vdata;


void main()
{
    vdata.mvp = cameraToView * worldToCamera * modelToWorld;
    vdata.normal = vec4(normal*10,0.0);
    vdata.position = vec4(position,1.0);
    //vColor = basicColor;
}
