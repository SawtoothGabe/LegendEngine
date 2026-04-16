#pragma once

#include <LE/Common/UID.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/Explicit/ExplicitResources.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    class ExplicitRenderer final : public Renderer
    {
    public:
        explicit ExplicitRenderer(ExplicitResources& resources);
        ~ExplicitRenderer() override;

        void StartFrame() override;
        void RenderFrame(RenderTargetID& target, std::span<Scene*> scenes) override;
        void EndFrame() override;

        [[nodiscard]] ExplicitDriver& GetDriver() const;
    private:
        void CreateCommandBuffers();
        void CreateSyncObjects();

        void UseMaterial(const Ref<Material>& material);
        void RenderScene(Scene& scene);
        void BeginScene(const Scene& scene);
        void DrawMesh(const Mesh& mesh, const Transform& transform);

        static void UpdateSceneUniforms(Scene& scene);
        static void UpdateCamera(Scene& scene, UID cameraID);

        ExplicitResources& m_resources;
        ExplicitDriver& m_driver;

        bool m_vsync = false;

        std::vector<CommandBufferID> m_commandBuffers;
        std::vector<FenceID> m_inFlightFences;
        std::vector<SemaphoreID> m_renderFinishedSemaphores;
        std::vector<RenderTargetID> m_targetsRendered;

        DescriptorSetID m_sets[3] = {};
        bool m_haveSetsChanged = true;
        Ref<Shader> m_currentShader = nullptr;

        Ref<Material> m_defaultMaterial;

        size_t m_currentFrame = 0;
    };
}
