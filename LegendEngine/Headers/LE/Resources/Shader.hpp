#pragma once

#include <LE/Common/Types.hpp>
#include <LE/Graphics/Types.hpp>
#include <LE/Resources/Resource.hpp>

namespace le
{
    class GraphicsResources;
    class Shader : public Resource
    {
        struct Passkey {};
    public:
        explicit Shader(GraphicsResources& resources, ShaderID handle, Passkey);
        ~Shader() override;

        void SetCullMode(CullMode cullMode);
        [[nodiscard]] CullMode GetCullMode() const;

        [[nodiscard]] ShaderID GetHandle() const;

        static Ref<Shader> Create(const sh::ShaderInfo& info);
    private:
        GraphicsResources& m_resources;
        ShaderID m_handle;

        CullMode m_cullMode = CullMode::BACK;
    };
}