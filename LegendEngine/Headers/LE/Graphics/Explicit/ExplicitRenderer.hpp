#pragma once

#include <LE/Graphics/GraphicsDriver.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    class ExplicitRenderer final : public Renderer
    {
    public:
        ExplicitRenderer(GraphicsDriver& driver, const CommandPoolID& gfxPool);
        ~ExplicitRenderer() override;

        Ref<Material> CreateMaterial() override;
        Ref<MeshData> CreateMeshData() override;
        Ref<Shader> CreateShader() override;
        Ref<Texture2D> CreateTexture2D() override;
        Ref<Texture2DArray> CreateTexture2DArray() override;
        Ref<RenderTarget> CreateRenderTarget() override;

        void StartFrame() override;
        void RenderFrame(RenderTargetID& target, std::span<Scene*> scenes) override;
        void EndFrame() override;

        void EnqueueDeletionFunc(const std::function<void()>& func);
    private:
        void CreateCommandBuffers() const;
        void CreateSyncObjects();

        void ProcessDeletionQueue();

        void BeginScene();
        void UseMaterial();
        void DrawMesh();

        GraphicsDriver& m_driver;
        CommandPoolID m_gfxPool;

        bool m_vsync = false;

        std::vector<CommandBufferID> m_commandBuffers;
        std::vector<FenceID> m_inFlightFences;
        std::vector<SemaphoreID> m_renderFinishedSemaphores;

        std::vector<std::vector<std::function<void()>>> m_deletionQueues;

        DescriptorSetID m_sets[3] = {};
        bool m_haveSetsChanged = true;
        Ref<Shader> m_currentShader = nullptr;

        size_t m_currentFrame = 0;
    };
}
