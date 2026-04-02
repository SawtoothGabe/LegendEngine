#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Graphics/ShaderManager.hpp>
#include <LE/Graphics/API/DynamicUniforms.hpp>
#include <LE/Math/Types.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/Resources/Texture.hpp>

namespace le
{
    class Shader;

    Material::Material()
        :
        m_context(Application::Get().GetGraphicsContext()),
        m_resourceManager(Application::Get().GetResourceManager()),
        m_graphicsResources(Application::Get().GetGraphicsResources()),
        m_ShaderManager(Application::Get().GetGraphicsResources().GetShaderManager()),
        m_shaderId(m_ShaderManager.GetByID("solid"))
    {
        m_buffer = m_context.CreatePerFrameBuffer(Buffer::UsageFlags::UNIFORM_BUFFER,
            sizeof(Uniforms));

        m_uniforms = m_context.CreateDynamicUniforms(
            DynamicUniforms::UpdateFrequency::PER_FRAME, m_graphicsResources.GetMaterialLayout());
    }

    void Material::SetTexture(const ID<Texture>& toSet)
    {
        m_textureId = toSet;

        if (toSet != 0)
            m_shaderId = m_ShaderManager.GetByID("textured");
        else
            m_shaderId = m_ShaderManager.GetByID("solid");

        m_uniforms->Invalidate();
    }

    void Material::SetColor(const Color& toSet)
    {
        m_uniformData.color = toSet;
    }

    Resource::ID<Texture> Material::GetTexture() const
    {
        return m_textureId;
    }

    Resource::ID<Shader> Material::GetShader() const
    {
        return m_shaderId;
    }

    Color Material::GetColor() const
    {
        return m_uniformData.color;
    }

    void Material::UpdateUniforms()
    {
        if (m_lastUpdatedFrame == Application::Get().GetCurrentFrame())
            return;

        m_uniforms->UpdateUniformBuffer(*m_buffer, 0);

        if (m_textureId != 0)
        {
            const Ref<Texture> texture = m_resourceManager.GetResource<Texture>(m_textureId);
            Image& image = texture->GetImage();
            m_uniforms->UpdateCombinedImageSampler(image, m_graphicsResources.GetSampler(), 1);
        }

        m_lastUpdatedFrame = Application::Get().GetCurrentFrame();
    }

    void Material::CopyUniformData() const
    {
        m_buffer->Update(sizeof(m_uniformData), 0, &m_uniformData);
    }

    DynamicUniforms& Material::GetUniforms() const
    {
        return *m_uniforms;
    }
}
