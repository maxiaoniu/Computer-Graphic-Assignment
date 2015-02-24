#version 410
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec4 vColor;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

void main()
{
    gl_Position = cameraToView * modelToWorld * vec4(position, 1.0);//in shader program, you must assign some values to gl_Position
    vColor = vec4(color, 1.0);
}
