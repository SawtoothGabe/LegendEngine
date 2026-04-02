#pragma once

#include <LE/Graphics/ShaderManager.hpp>
#include <LE/Graphics/API/DescriptorSetLayout.hpp>
#include <LE/Graphics/API/Sampler.hpp>

#include <vector>

namespace le
{
    class Material;
    class GraphicsContext;
    class ResourceManager;
    class GraphicsResources final
    {
    public:
        GraphicsResources(GraphicsContext& context);

        [[nodiscard]] ShaderManager& GetShaderManager();
        [[nodiscard]] Sampler& GetSampler();
        [[nodiscard]] Material& GetDefaultMaterial() const;

        DescriptorSetLayout& GetCameraLayout() const;
        DescriptorSetLayout& GetMaterialLayout() const;
        DescriptorSetLayout& GetSceneLayout() const;
        std::span<DescriptorSetLayout*> GetLayouts();

        void CreateResources(ResourceManager& resourceManager);
    private:
        void CreateSampler();
        void CreateCameraDescriptorSetLayout();
        void CreateSceneDescriptorSetLayout();
        void CreateMaterialDescriptorSetLayout();

        GraphicsContext& m_context;

        ShaderManager m_shaderManager;
        Scope<Sampler> m_sampler;

        Ref<Material> m_defaultMaterial;

        Scope<DescriptorSetLayout> m_cameraLayout = nullptr;
        Scope<DescriptorSetLayout> m_sceneLayout = nullptr;
        Scope<DescriptorSetLayout> m_materialLayout = nullptr;
        std::vector<DescriptorSetLayout*> m_setLayouts;
    };
}
