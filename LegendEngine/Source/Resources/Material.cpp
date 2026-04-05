#include <LE/Application.hpp>
#include <LE/Graphics/ShaderManager.hpp>
#include <LE/Math/Types.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/Resources/Texture.hpp>

namespace le
{
    class Shader;

    Material::Material()
        :
        m_ShaderManager(Application::Get().GetGraphicsContext().GetShaderManager()),
        m_shaderId(m_ShaderManager.GetByID("solid"))
    {
    }

    void Material::SetTexture(const Ref<Texture>& toSet)
    {
        m_textureId = toSet;

        if (toSet != 0)
            m_shaderId = m_ShaderManager.GetByID("textured");
        else
            m_shaderId = m_ShaderManager.GetByID("solid");
    }

    void Material::SetColor(const Color& toSet)
    {
        m_uniformData.color = toSet;
    }

    Ref<Texture> Material::GetTexture() const
    {
        return m_textureId;
    }

    Ref<Shader> Material::GetShader() const
    {
        return m_shaderId;
    }

    Color Material::GetColor() const
    {
        return m_uniformData.color;
    }
}
