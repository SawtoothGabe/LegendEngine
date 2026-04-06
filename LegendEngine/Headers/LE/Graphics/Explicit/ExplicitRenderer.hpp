#pragma once

#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    class ExplicitRenderer final : public Renderer
    {
    public:
        ExplicitRenderer(Scope<ExplicitDriver> driver, const CommandPoolID& gfxPool);
        ~ExplicitRenderer() override;

        MaterialID CreateMaterial() override;
        MeshID CreateMesh() override;
        ShaderID CreateShader() override;
        Texture2DID CreateTexture2D() override;
        Texture2DArrayID CreateTexture2DArray() override;
        RenderTargetID CreateRenderTarget(Window& window) override;

        void DestroyMaterial(MaterialID id) override;
        void DestroyMesh(MeshID id) override;
        void DestroyShader(ShaderID id) override;
        void DestroyTexture2D(Texture2DID id) override;
        void DestroyTexture2DArray(Texture2DArrayID id) override;
        void DestroyRenderTarget(RenderTargetID id) override;

        void StartFrame() override;
        void RenderFrame(RenderTargetID& target, std::span<Scene*> scenes) override;
        void EndFrame() override;

        void EnqueueDeletionFunc(const std::function<void()>& func);

        ExplicitDriver& GetDriver() const;
    private:
        void CreateCommandBuffers();
        void CreateSyncObjects();

        void ProcessDeletionQueue();

        void BeginScene();
        void UseMaterial();
        void DrawMesh();

        Scope<ExplicitDriver> m_driver;
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
