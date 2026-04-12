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
    float4x4 vpMat;
};

struct Object
{
    float4x4 transform;
};

struct Material
{
    float4 color;
    Sampler2D albedo;
};

struct Light
{
    float4 position;
    float4 color;
    uint type;
};

struct Scene
{
    float ambientLight;
};

#define OBJECT_DATA [[vk::push_constant]]
#define CAMERA_SET [[vk::binding(0, 0)]]
#define SCENE_SET [[vk::binding(0, 1)]]
#define SCENE_LIGHTS [[vk::binding(1, 1)]]
#define MATERIAL_SET [[vk::binding(0, 2)]]
