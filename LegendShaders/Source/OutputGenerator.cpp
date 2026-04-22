#include "OutputGenerator.hpp"

#include <print>
#include <SlangUtils.hpp>

static void WriteBytes(std::string& output, const size_t count, const void* data)
{
    const auto bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < count; ++i)
    {
        if (constexpr size_t lineWidth = 15; i % lineWidth == 0 && i != count - 1)
        {
            output += "\n";
            output += "\t";
        }

        output += std::format("0x{:02x}, ", bytes[i]);
    }
}

static void AppendDXIL(std::string& output, const Program& program, slang::IComponentType* comp, size_t entrypointIndex)
{
    Slang::ComPtr<slang::IBlob> diagnostics;
    Slang::ComPtr<slang::IBlob> code;

    // targetIndex is always zero since DXIL is added to the targets first in Options.cpp
    // Although, because of a limitation of DXIL, you must have a separate blob per entry point.
    comp->getEntryPointCode(entrypointIndex, 0, code.writeRef(), diagnostics.writeRef());
    SlangUtils::Diagnose(diagnostics);

    if (!code)
        throw std::runtime_error("getEntryPointCode returned nullptr code. DXIL is only available with MSVC on Windows.");

    output += std::format("static uint8_t SHADER_{}_ENTRYPOINT{}_DXIL[] = \n{{",
        program.GetFilenameHash(), entrypointIndex);

    WriteBytes(output, code->getBufferSize(), code->getBufferPointer());

    output += "\n};\n\n";
}

static void AppendGLSL(std::string& output, const Program& program, slang::IComponentType* comp, size_t entrypointIndex, size_t targetIndex)
{
    Slang::ComPtr<slang::IBlob> diagnostics;
    Slang::ComPtr<slang::IBlob> code;

    // The idea for DXIL for the most part exists in OpenGL as well, so these have to be per-entrypoint.
    // ...and also because slang likes to generate two function called main if there are two glsl entrypoints
    comp->getEntryPointCode(entrypointIndex, targetIndex, code.writeRef(), diagnostics.writeRef());
    SlangUtils::Diagnose(diagnostics);

    output += std::format("static const char* SHADER_{}_ENTRYPOINT{}_GLSL = R\"(\n{})\";\n\n",
        program.GetFilenameHash(), entrypointIndex, static_cast<const char*>(code->getBufferPointer()));
}

static Slang::ComPtr<slang::IBlob> GetCode(size_t index, slang::IComponentType* comp)
{
    Slang::ComPtr<slang::IBlob> diagnostics;
    Slang::ComPtr<slang::IBlob> code;
    comp->getTargetCode(index, code.writeRef(), diagnostics.writeRef());
    SlangUtils::Diagnose(diagnostics);

    return code;
}

static void AppendNextTargetBytes(std::string& output, slang::IComponentType* comp, const size_t targetIndex,
    std::string_view symbolName)
{
    const Slang::ComPtr<slang::IBlob> code = GetCode(targetIndex, comp);

    output += std::format("static uint8_t {}[] = \n{{", symbolName);
    WriteBytes(output, code->getBufferSize(), code->getBufferPointer());
    output += "\n};\n\n";
}

static void AppendNextTargetString(std::string& output, slang::IComponentType* comp, const size_t targetIndex,
    std::string_view symbolName)
{
    const Slang::ComPtr<slang::IBlob> code = GetCode(targetIndex, comp);
    output += std::format("static const char* {} = R\"(\n{})\";\n\n", symbolName,
        static_cast<const char*>(code->getBufferPointer()));
}

static std::string GetStageName(const SlangStage stage)
{
    switch (stage)
    {
        case SLANG_STAGE_NONE: return "le::ShaderStage::NONE";
        case SLANG_STAGE_VERTEX: return "le::ShaderStage::VERTEX";
        case SLANG_STAGE_HULL: return "le::ShaderStage::HULL";
        case SLANG_STAGE_DOMAIN: return "le::ShaderStage::DOMAIN_STAGE";
        case SLANG_STAGE_GEOMETRY: return "le::ShaderStage::GEOMETRY";
        case SLANG_STAGE_FRAGMENT: return "le::ShaderStage::FRAGMENT";
        case SLANG_STAGE_COMPUTE: return "le::ShaderStage::COMPUTE";
        case SLANG_STAGE_RAY_GENERATION: return "le::ShaderStage::RAY_GENERATION";
        case SLANG_STAGE_INTERSECTION: return "le::ShaderStage::INTERSECTION";
        case SLANG_STAGE_ANY_HIT: return "le::ShaderStage::ANY_HIT";
        case SLANG_STAGE_CLOSEST_HIT: return "le::ShaderStage::CLOSEST_HIT";
        case SLANG_STAGE_MISS: return "le::ShaderStage::MISS";
        case SLANG_STAGE_CALLABLE: return "le::ShaderStage::CALLABLE";
        case SLANG_STAGE_MESH: return "le::ShaderStage::MESH";
        case SLANG_STAGE_AMPLIFICATION: return "le::ShaderStage::AMPLIFICATION";
        case SLANG_STAGE_DISPATCH: return "le::ShaderStage::DISPATCH";
        case SLANG_STAGE_COUNT: return "le::ShaderStage::COUNT";
    }

    return "";
}

static std::string AddEntrypoints(const Options& options, std::string& output, Program& program,
    const Slang::ComPtr<slang::IComponentType>& comp)
{
    std::string entrypointNames;
    const std::vector<Slang::ComPtr<slang::IEntryPoint>>& entrypoints = program.GetEntrypoints();
    for (size_t i = 0; i < entrypoints.size(); ++i)
    {
        slang::EntryPointReflection* pReflection = comp->getLayout()->getEntryPointByIndex(i);

        if (options.flags & Options::CompileFlagBits::DXIL)
            AppendDXIL(output, program, comp, i);

        std::string symbolName = std::format(
            "SHADER_{}_ENTRYPOINT{}",
            program.GetFilenameHash(),
            i
        );

        std::string glslName;
        if (options.flags & Options::CompileFlagBits::GLSL)
        {
            glslName = std::format("{}_GLSL", symbolName);
            AppendGLSL(output, program, comp, i, options.flags & Options::CompileFlagBits::DXIL);
        }

        entrypointNames += std::format("\t{},\n", symbolName);
        output += std::format("static le::Entrypoint {} = \n{{\n", symbolName);
        output += std::format("\t.stage = {},\n", GetStageName(pReflection->getStage()));
        output += std::format("\t.pName = \"{}\",\n", pReflection->getName());

        if (options.flags & Options::CompileFlagBits::DXIL)
        {
            output += std::format("\t.dxilCodeSize = sizeof({}_DXIL),\n", symbolName);
            output += std::format("\t.pDxilCode = {}_DXIL,\n", symbolName);
        }

        if (options.flags & Options::CompileFlagBits::GLSL)
            output += std::format("\t.pGlslCode = {},\n", glslName);

        output += "};\n\n";
    }

    return entrypointNames;
}

std::string OutputGenerator::LinkProgramsAndMakeOutput(const Options& options, slang::ISession* session,
                                                       std::vector<Program>& programs)
{
    std::string output;
    output += "#include <LE/Graphics/ShaderInfo.hpp>\n";
    output += "\n";

    std::vector<Slang::ComPtr<slang::IComponentType>> linkedPrograms;
    linkedPrograms.reserve(programs.size());

    for (Program& program : programs)
        linkedPrograms.push_back(program.Link(session));

    for (size_t i = 0; i < linkedPrograms.size(); ++i)
    {
        Program& program = programs[i];
        auto linkedProgram = linkedPrograms[i];
        std::string filenameHash = program.GetFilenameHash();

        output += std::format("static le::Entrypoint SHADER_{}_ENTRYPOINTS[] = \n{{\n{}}};\n\n",
            filenameHash,
            AddEntrypoints(options, output, program, linkedProgram)
        );

        static_assert(static_cast<int>(Options::CompileFlagBits::DXIL) == 1);
        size_t targetIndex = options.flags & Options::CompileFlagBits::DXIL;
        targetIndex += options.flags & Options::CompileFlagBits::GLSL ? 1 : 0;

        if (options.flags & Options::CompileFlagBits::SPIRV)
            AppendNextTargetBytes(output, linkedProgram, targetIndex++,
                std::format("SHADER_{}_SPIRV", filenameHash));

        if (options.flags & Options::CompileFlagBits::WGSL)
            AppendNextTargetString(output, linkedProgram, targetIndex,
                std::format("SHADER_{}_WGSL", filenameHash));
    }

    for (Program& program : programs)
    {
        output += std::format("le::ShaderInfo LE_SHADER_{}_INFO = \n{{\n", program.GetFilenameHash());
        output += std::format("\t.name = \"{}\",\n", program.GetName());
        output += std::format("\t.features = {},\n", static_cast<uint64_t>(program.GetFeatures()));
        output += std::format("\t.entrypointCount = {},\n", program.GetEntrypoints().size());
        output += std::format("\t.pEntrypoints = SHADER_{}_ENTRYPOINTS,\n", program.GetFilenameHash());

        if (options.flags & Options::CompileFlagBits::SPIRV)
        {
            output += std::format("\t.spirvCodeSize = sizeof(SHADER_{}_SPIRV),\n", program.GetFilenameHash());
            output += std::format("\t.pSpirvCode = SHADER_{}_SPIRV,\n", program.GetFilenameHash());
        }

        if (options.flags & Options::CompileFlagBits::WGSL)
            output += std::format("\t.pWgslCode = SHADER_{}_WGSL,\n", program.GetFilenameHash());

        output += "};\n\n";
        output += std::format("LE_REGISTER_SHADER(LE_SHADER_{}_INFO);\n\n", program.GetFilenameHash());
    }

    if (output.empty())
        std::println("warn: generated output is empty");

    return output;
}
