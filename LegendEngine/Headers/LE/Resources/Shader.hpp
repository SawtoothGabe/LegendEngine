#pragma once

#include <span>
#include <LE/Common/Types.hpp>
#include <LE/Graphics/API/Pipeline.hpp>
#include <LE/Resources/Resource.hpp>

namespace le
{
    class Shader final : public Resource
    {
    public:
        enum class CullMode
        {
            NONE,
            BACK,
            FRONT,
        };

        struct Stage
        {
            ShaderType type = ShaderType::VERTEX;
            const uint8_t* compiledShaderCode = nullptr;
            size_t compiledShaderSize = 0;
        };

        explicit Shader(std::span<Stage> stages);

        void SetCullMode(CullMode cullMode);
        [[nodiscard]] CullMode GetCullMode() const;

        [[nodiscard]] Pipeline& GetPipeline() const;

        ID<Shader> id = ID<Shader>(m_uid);
    protected:
        CullMode m_CullMode = CullMode::BACK;

        Scope<Pipeline> m_pipeline;
    };
}