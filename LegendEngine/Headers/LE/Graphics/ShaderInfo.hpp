#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

namespace le
{
    enum class ShaderStage
    {
        NONE,
        VERTEX,
        HULL,
        DOMAIN_STAGE, // Called DOMAIN_STAGE and not DOMAIN because MSVC #defines that
        GEOMETRY,
        FRAGMENT,
        COMPUTE,
        RAY_GENERATION,
        INTERSECTION,
        ANY_HIT,
        CLOSEST_HIT,
        MISS,
        CALLABLE,
        MESH,
        AMPLIFICATION,
        DISPATCH,
        COUNT
    };

    enum class Features
    {
        SOLID_COLOR = 1 << 0,
        TEXTURED    = 1 << 1,
    };

    struct Entrypoint
    {
        ShaderStage stage = ShaderStage::VERTEX;
        const char* pName = nullptr;
        size_t dxilCodeSize = 0;
        uint8_t* pDxilCode = nullptr;

        // null terminated
        const char* pGlslCode = nullptr;
    };

    struct ShaderInfo
    {
        const char* name = nullptr;
        uint64_t features = 0;
        size_t entrypointCount = 0;
        Entrypoint* pEntrypoints = nullptr;
        size_t spirvCodeSize = 0;
        uint8_t* pSpirvCode = nullptr;

        // null terminated
        const char* pWgslCode = nullptr;
    };

    class ShaderRegistry
    {
    public:
        void Register(ShaderInfo* pInfo);
        ShaderInfo* GetShader(std::string_view name);
        ShaderInfo* FromFeatures(uint64_t features);

        static ShaderRegistry& Get();
    private:
        std::unordered_map<std::string, ShaderInfo*> m_shaders;
        std::unordered_map<uint64_t, ShaderInfo*> m_featuredShaders;
    };

#define LE_REGISTER_SHADER(shaderInfo) \
    static int LE_SHADER_REG_##shaderInfo = (le::ShaderRegistry::Get().Register(&shaderInfo), 0)
}
