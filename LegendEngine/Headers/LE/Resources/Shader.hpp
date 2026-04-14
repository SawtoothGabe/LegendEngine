#pragma once

#include <span>
#include <LE/Common/Types.hpp>
#include <LE/Resources/Resource.hpp>

#include <LE/Graphics/Types.hpp>

namespace le
{
    class Shader : public Resource
    {
    public:
        explicit Shader();

        void SetCullMode(CullMode cullMode);
        [[nodiscard]] CullMode GetCullMode() const;

        ShaderID GetHandle() const;
    protected:
        CullMode m_CullMode = CullMode::BACK;
    };
}