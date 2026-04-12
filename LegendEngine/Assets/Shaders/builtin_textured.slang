import common;

[[vk::push_constant]]
Object obj;

[[vk::binding(0, 0)]]
ParameterBlock<Camera> cam;

[[vk::binding(0, 1)]]
ParameterBlock<Scene> scene;

[[vk::binding(1, 1)]]
StructuredBuffer<Light> lights;

[[vk::binding(0, 2)]]
ParameterBlock<Material> mat;

struct VSOutput
{
    float4 pos : SV_Position;
    float2 coord : TEXCOORD0;
    float3 worldPos;
};

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    float4 worldPos = mul(obj.transform, float4(input.pos, 1));

    VSOutput output;
    output.pos = mul(cam.vpMat, worldPos);
    output.worldPos = worldPos.xyz;
    output.coord = input.texcoord;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
    float3 finalLight = float3(scene.ambientLight);

    uint numLights;
    uint stride;
    lights.GetDimensions(numLights, stride);

    for (uint i = 0; i < numLights; i++)
    {
        Light light = lights[i];

        float intensity = 1 / pow(distance(input.worldPos, light.position.xyz), 2);
        float3 color = light.color.xyz * intensity;

        color *= light.color.w;
        finalLight += color;
    }

    float3 finalColor = mat.albedo.Sample(input.coord).xyz;
    finalColor *= mat.color.xyz;
    finalColor *= finalLight;

    return float4(finalColor, 1);
}