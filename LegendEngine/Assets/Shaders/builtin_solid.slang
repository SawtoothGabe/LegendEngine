import common;

OBJECT_DATA Object obj;
CAMERA_SET ParameterBlock<Camera> cam;
MATERIAL_SET ParameterBlock<Material> mat;

[shader("vertex")]
float4 vertMain(VSInput input) : SV_Position
{
    return cam.vpMat * obj.transform * float4(input.pos, 1);
}

[shader("fragment")]
float4 fragMain() : SV_Target
{
    return float4(mat.color, 1);
}