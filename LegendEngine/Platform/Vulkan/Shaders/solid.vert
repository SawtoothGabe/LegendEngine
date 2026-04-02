#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 fragCoord;

layout(set = 0, binding = 0) uniform Camera
{
    mat4 cameraMatrix;
} cam;

layout(push_constant) uniform Object
{
    mat4 transform;
} obj;

void main() 
{
    fragCoord = texcoord;
    gl_Position = cam.cameraMatrix * obj.transform * vec4(pos.xyz, 1);
}