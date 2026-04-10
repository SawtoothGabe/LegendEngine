import common;

OBJECT_DATA Object obj;
SCENE_SET ParameterBlock<Scene> scene;
CAMERA_SET ParameterBlock<Camera> cam;
MATERIAL_SET ParameterBlock<Material> mat;

struct VSOutput
{
    float4 pos : SV_Position;
    float2 coord : TEXCOORD0;
    float3 worldPos;
};

[shader("vertex")]
VSOutput vertMain(VSInput input)
{
    float4 worldPos = obj.transform * float4(input.pos, 1);

    VSOutput output;
    output.pos = cam.vpMat * worldPos;
    output.worldPos = worldPos;
    output.coord = input.texcoord;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput input) : SV_Target
{
    float3 finalLight = float3(scene.data.ambientLight);
    for (uint i = 0; i < scene.data.lightCount; i++)
    {
        Light light = scene.data.lights[i];

        float intensity = 1 / pow(distance(input.worldPos, light.position), 2);
        float3 color = float3(light.color) * intensity;

        color *= light.color.w;
        finalLight += color;
    }

    float3 finalColor = mat.albedo.Sample(input.coord);
    finalColor *= float3(mat.color);
    finalColor *= finalLight;

    return float4(finalColor, 1);
}