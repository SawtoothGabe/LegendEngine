#version 450

const uint LIGHT_TYPE_POINT = 0;
const uint LIGHT_TYPE_SPOT  = 1;
const uint LIGHT_TYPE_SUN   = 2;

struct Light
{
    vec4 position;
    vec4 color;
    uint type;
};

layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec3 inPos;
layout(location = 2) in vec3 inWorldPos;

layout(location = 0) out vec4 fragColor;

layout(set = 1, binding = 0) readonly buffer Scene
{
    float ambientLight;
    Light lights[];
} scene;

layout(set = 2, binding = 0) uniform Material
{
    vec4 color;
} mat;

layout(set = 2, binding = 1) uniform sampler2D albedoSampler;

void main()
{
    vec3 finalLight = vec3(scene.ambientLight);
    for (uint i = 0; i < scene.lights.length(); i++)
    {
        Light data = scene.lights[i];

        float intensity = 1 / pow(distance(inWorldPos, data.position.xyz), 2);

        vec3 light = data.color.xyz * intensity;
        light *= data.color.w;

        finalLight += light;
    }

    vec3 finalColor = texture(albedoSampler, inCoord.xy).xyz;
    finalColor *= mat.color.xyz;
    finalColor *= finalLight;

    fragColor = vec4(finalColor.xyz, 1.0);
}