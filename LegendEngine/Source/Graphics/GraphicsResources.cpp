#include <LE/Application.hpp>
#include <LE/Graphics/GraphicsResources.hpp>
#include <LE/Resources/Material.hpp>

namespace le
{
    GraphicsResources::GraphicsResources(GraphicsContext& context)
        :
        m_context(context)
    {
        CreateSampler();

        // This order matters
        CreateCameraDescriptorSetLayout();
        CreateSceneDescriptorSetLayout();
        CreateMaterialDescriptorSetLayout();
    }

    ShaderManager& GraphicsResources::GetShaderManager()
    {
        return m_shaderManager;
    }

    Sampler& GraphicsResources::GetSampler()
    {
        return *m_sampler;
    }

    Material& GraphicsResources::GetDefaultMaterial() const
    {
        return *m_defaultMaterial;
    }

    DescriptorSetLayout& GraphicsResources::GetCameraLayout() const
    {
        return *m_cameraLayout;
    }

    DescriptorSetLayout& GraphicsResources::GetMaterialLayout() const
    {
        return *m_materialLayout;
    }

    DescriptorSetLayout& GraphicsResources::GetSceneLayout() const
    {
        return *m_sceneLayout;
    }

    std::span<DescriptorSetLayout*> GraphicsResources::GetLayouts()
    {
        return m_setLayouts;
    }

    void GraphicsResources::CreateCameraDescriptorSetLayout()
    {
        constexpr DescriptorSetLayout::Binding cameraBinding {
            .binding = 0,
            .descriptorCount = 1,
            .descriptorType = DescriptorType::UNIFORM_BUFFER,
            .stageFlags = ShaderStageFlags::VERTEX
        };

        DescriptorSetLayout::Binding bindings[] = {
            cameraBinding
        };

        m_cameraLayout = m_context.CreateDescriptorSetLayout(std::span(bindings));
        m_setLayouts.push_back(m_cameraLayout.get());
    }

    void GraphicsResources::CreateSceneDescriptorSetLayout()
    {
        constexpr DescriptorSetLayout::Binding sceneBinding {
            .binding = 0,
            .descriptorCount = 1,
            .descriptorType = DescriptorType::STORAGE_BUFFER,
            .stageFlags = ShaderStageFlags::FRAGMENT
        };

        DescriptorSetLayout::Binding bindings[] = {
            sceneBinding
        };

        m_sceneLayout = m_context.CreateDescriptorSetLayout(std::span(bindings));
        m_setLayouts.push_back(m_sceneLayout.get());
    }

    void GraphicsResources::CreateMaterialDescriptorSetLayout()
    {
        constexpr DescriptorSetLayout::Binding uniformBinding {
            .binding = 0,
            .descriptorCount = 1,
            .descriptorType = DescriptorType::UNIFORM_BUFFER,
            .stageFlags = ShaderStageFlags::FRAGMENT
        };

        constexpr DescriptorSetLayout::Binding samplerBinding {
            .binding = 1,
            .descriptorCount = 1,
            .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
            .stageFlags = ShaderStageFlags::FRAGMENT
        };

        DescriptorSetLayout::Binding bindings[] = {
            uniformBinding,
            samplerBinding
        };

        m_materialLayout = m_context.CreateDescriptorSetLayout(std::span(bindings));
        m_setLayouts.push_back(m_materialLayout.get());
    }

    void GraphicsResources::CreateResources(ResourceManager& resourceManager)
    {
        m_defaultMaterial = resourceManager.CreateResource<Material>();
    }

    void GraphicsResources::CreateSampler()
    {
        constexpr Sampler::Info info {
            .filter = Sampler::Filter::NEAREST,
            .addressMode = Sampler::AddressMode::CLAMP_TO_EDGE,
            .borderColor = Sampler::BorderColor::OPAQUE_BLACK
        };

        m_sampler = m_context.CreateSampler(info);
    }
}
