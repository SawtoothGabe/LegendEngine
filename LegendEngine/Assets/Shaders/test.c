struct _MatrixStorage_float4x4std140_0
{
    @align(16) data_0 : array<vec4<f32>, i32(4)>,
};

struct Camera_std140_0
{
    @align(16) vpMat_0 : _MatrixStorage_float4x4std140_0,
};

@binding(0) @group(0) var<uniform> cam_0 : Camera_std140_0;
struct _MatrixStorage_float4x4std430_0
{
    @align(16) data_1 : array<vec4<f32>, i32(4)>,
};

struct Object_std430_0
{
    @align(16) transform_0 : _MatrixStorage_float4x4std430_0,
};

var<uniform> obj_0 : Object_std430_0;
struct Material_std140_0
{
    @align(16) color_0 : vec4<f32>,
};

@binding(0) @group(1) var<uniform> mat_0 : Material_std140_0;
struct vertexOutput_0
{
    @builtin(position) output_0 : vec4<f32>,
};

struct vertexInput_0
{
    @location(0) pos_0 : vec3<f32>,
    @location(1) texcoord_0 : vec2<f32>,
};

@vertex
fn vertMain( _S1 : vertexInput_0) -> vertexOutput_0
{
    var _S2 : vertexOutput_0 = vertexOutput_0( (((vec4<f32>(_S1.pos_0, 1.0f)) * ((((mat4x4<f32>(obj_0.transform_0.data_1[i32(0)][i32(0)], obj_0.transform_0.data_1[i32(0)][i32(1)], obj_0.transform_0.data_1[i32(0)][i32(2)], obj_0.transform_0.data_1[i32(0)][i32(3)], obj_0.transform_0.data_1[i32(1)][i32(0)], obj_0.transform_0.data_1[i32(1)][i32(1)], obj_0.transform_0.data_1[i32(1)][i32(2)], obj_0.transform_0.data_1[i32(1)][i32(3)], obj_0.transform_0.data_1[i32(2)][i32(0)], obj_0.transform_0.data_1[i32(2)][i32(1)], obj_0.transform_0.data_1[i32(2)][i32(2)], obj_0.transform_0.data_1[i32(2)][i32(3)], obj_0.transform_0.data_1[i32(3)][i32(0)], obj_0.transform_0.data_1[i32(3)][i32(1)], obj_0.transform_0.data_1[i32(3)][i32(2)], obj_0.transform_0.data_1[i32(3)][i32(3)])) * (mat4x4<f32>(cam_0.vpMat_0.data_0[i32(0)][i32(0)], cam_0.vpMat_0.data_0[i32(0)][i32(1)], cam_0.vpMat_0.data_0[i32(0)][i32(2)], cam_0.vpMat_0.data_0[i32(0)][i32(3)], cam_0.vpMat_0.data_0[i32(1)][i32(0)], cam_0.vpMat_0.data_0[i32(1)][i32(1)], cam_0.vpMat_0.data_0[i32(1)][i32(2)], cam_0.vpMat_0.data_0[i32(1)][i32(3)], cam_0.vpMat_0.data_0[i32(2)][i32(0)], cam_0.vpMat_0.data_0[i32(2)][i32(1)], cam_0.vpMat_0.data_0[i32(2)][i32(2)], cam_0.vpMat_0.data_0[i32(2)][i32(3)], cam_0.vpMat_0.data_0[i32(3)][i32(0)], cam_0.vpMat_0.data_0[i32(3)][i32(1)], cam_0.vpMat_0.data_0[i32(3)][i32(2)], cam_0.vpMat_0.data_0[i32(3)][i32(3)]))))))) );
    return _S2;
}

struct pixelOutput_0
{
    @location(0) output_1 : vec4<f32>,
};

@fragment
fn fragMain() -> pixelOutput_0
{
    var _S3 : pixelOutput_0 = pixelOutput_0( vec4<f32>(mat_0.color_0.xyz, 1.0f) );
    return _S3;
}

