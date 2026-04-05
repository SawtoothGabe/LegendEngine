#pragma once

#include <LE/Graphics/GraphicsDriver.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    class ExplicitRenderer final : public Renderer
    {
    public:
        ExplicitRenderer(GraphicsDriver& driver, CommandPoolID gfxPool);
        ~ExplicitRenderer();

        MaterialHandle CreateMaterial() override;
        MeshHandle CreateMesh() override;
        ShaderHandle CreateShader() override;
        Texture2DHandle CreateTexture2D() override;
        Texture2DArrayHandle CreateTexture2DArray() override;
        RenderTargetHandle CreateRenderTarget() override;

        void StartFrame() override;
        void RenderFrame(RenderTarget& target, std::span<Scene*> scenes) override;
        void EndFrame() override;
    private:
        void CreateCommandBuffers();
        void CreateSyncObjects();

        void ProcessDeletionQueue();

        void BeginScene();
        void UseMaterial();
        void DrawMesh();

        GraphicsDriver& m_driver;
        CommandPoolID m_gfxPool;

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
