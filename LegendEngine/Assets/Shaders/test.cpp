#include <lesh/Shader.hpp>

static const char* SHADER_1caa27e45f383e30_ENTRYPOINT0_GLSL = R"(
#version 450
layout(column_major) uniform;
layout(column_major) buffer;

#line 14 0
struct Camera_0
{
    mat4x4 vpMat_0;
};


#line 7 1
layout(binding = 0)
layout(std140) uniform block_Camera_0
{
    mat4x4 vpMat_0;
}cam_0;

#line 19 0
struct Object_0
{
    mat4x4 transform_0;
};


#line 4 1
layout(push_constant)
layout(std430) uniform block_Object_0
{
    mat4x4 transform_0;
}obj_0;

#line 8 0
layout(location = 0)
in vec3 input_pos_0;


#line 13 1
void main()
{

#line 13
    gl_Position = (((vec4(input_pos_0, 1.0)) * ((((obj_0.transform_0) * (cam_0.vpMat_0))))));

#line 13
    return;
}

)";

static le::sh::Entrypoint SHADER_1caa27e45f383e30_ENTRYPOINT0 = 
{
	.stage = le::sh::ShaderStage::VERTEX,
	.pName = "vertMain",
	.pGlslCode = SHADER_1caa27e45f383e30_ENTRYPOINT0_GLSL,
};

static const char* SHADER_1caa27e45f383e30_ENTRYPOINT1_GLSL = R"(
#version 450
layout(column_major) uniform;
layout(column_major) buffer;

#line 24 0
struct Material_0
{
    vec4 color_0;
};


#line 27
layout(binding = 0, set = 1)
layout(std140) uniform block_Material_0
{
    vec4 color_0;
}mat_0;

#line 27
layout(location = 0)
out vec4 entryPointParam_fragMain_0;


#line 19 1
void main()
{

#line 19
    entryPointParam_fragMain_0 = vec4(mat_0.color_0.xyz, 1.0);

#line 19
    return;
}

)";

static le::sh::Entrypoint SHADER_1caa27e45f383e30_ENTRYPOINT1 = 
{
	.stage = le::sh::ShaderStage::FRAGMENT,
	.pName = "fragMain",
	.pGlslCode = SHADER_1caa27e45f383e30_ENTRYPOINT1_GLSL,
};

static le::sh::Entrypoint SHADER_1caa27e45f383e30_ENTRYPOINTS[] = 
{
	SHADER_1caa27e45f383e30_ENTRYPOINT0,
	SHADER_1caa27e45f383e30_ENTRYPOINT1,
};

extern "C"
{

le::sh::ShaderInfo LE_SHADER_1caa27e45f383e30_INFO = 
{
	.features = 1,
	.entrypointCount = 2,
	.pEntrypoints = SHADER_1caa27e45f383e30_ENTRYPOINTS,
};

}
