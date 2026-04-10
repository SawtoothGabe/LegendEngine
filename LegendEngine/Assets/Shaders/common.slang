enum LightType
{
    POINT = 0,
    SPOT  = 1,
    SUN   = 2
};

struct VSInput
{
    float3 pos : POSITION;
    float2 texcoord : TEXCOORD0;
};

struct Camera
{
    ConstantBuffer<float4x4> vpMat;
};

struct Object
{
    float4x4 transform;
};

struct Material
{
    ConstantBuffer<float4> color;
};

struct Light
{
    float4 position;
    float4 color;
    uint type;
};

struct SceneData
{
    float ambientLight;
    uint lightCount;
    Light lights[];
};

struct Scene
{
    StructuredBuffer<SceneData> data;
    Sampler2D albedo;
};

#define OBJECT_DATA [[vk::push_constant]]
#define CAMERA_SET [[vk::set(0)]]
#define SCENE_SET [[vk::set(1)]]
#define MATERIAL_SET [[vk::set(2)]]