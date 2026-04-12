import common;

[[vk::push_constant]]
Object obj;

[[vk::binding(0, 0)]]
ParameterBlock<Camera> cam;

[[vk::binding(0, 2)]]
ParameterBlock<Material> mat;

[shader("vertex")]
float4 vertMain(VSInput input) : SV_Position
{
    return mul(mul(cam.vpMat, obj.transform), float4(input.pos, 1));
}

[shader("fragment")]
float4 fragMain() : SV_Target
{
    return float4(mat.color.xyz, 1);
}
