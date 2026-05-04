#include "common.slang"

OBJECT_DATA
Object obj;

CAMERA_SET
ParameterBlock<Camera> cam;

MATERIAL_SET
ParameterBlock<Material> mat;

[shader("vertex")]
float4 vertMain(VSInput input) : SV_Position
{
    return mul(float4(input.pos, 1), mul(obj.transform, cam.vpMat));
}

[shader("fragment")]
float4 fragMain() : SV_Target
{
    return float4(mat.color.xyz, 1);
}
