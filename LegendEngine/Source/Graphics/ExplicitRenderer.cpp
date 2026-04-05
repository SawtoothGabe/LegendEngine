#include <LE/Application.hpp>
#include <LE/Graphics/Explicit/ExplicitMaterial.hpp>
#include <LE/Graphics/Explicit/ExplicitMeshData.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderTarget.hpp>
#include <LE/Graphics/Explicit/ExplicitShader.hpp>
#include <LE/Graphics/Explicit/ExplicitTexture2D.hpp>
#include <LE/Graphics/Explicit/ExplicitTexture2DArray.hpp>

namespace le
{
    ExplicitRenderer::ExplicitRenderer(GraphicsDriver& driver, const CommandPoolID& gfxPool)
        :
        m_driver(driver),
        m_gfxPool(gfxPool)
    {
        LE_INFO("Creating ExplicitRenderer");

        m_commandBuffers.resize(Application::FRAMES_IN_FLIGHT);
        m_renderFinishedSemaphores.resize(Application::FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(Application::FRAMES_IN_FLIGHT);
        m_deletionQueues.resize(Application::FRAMES_IN_FLIGHT);

        CreateCommandBuffers();
        CreateSyncObjects();

        LE_INFO("Created ExplicitRenderer");
    }

    ExplicitRenderer::~ExplicitRenderer()
    {
        LE_INFO("Destroying ExplicitRenderer");

        m_driver.WaitIdle();

        for (auto& queue : m_deletionQueues)
            for (auto& deletionFunc : queue)
                deletionFunc();

        m_driver.FreeCommandBuffers();

        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; ++i)
        {
            m_driver.DestroyFence(m_inFlightFences[i]);
            m_driver.DestroySemaphore(m_renderFinishedSemaphores[i]);
        }

        LE_INFO("Destroyed ExplicitRenderer");
    }

    Ref<Material> ExplicitRenderer::CreateMaterial()
    {
        return std::make_shared<ExplicitMaterial>();
    }

    Ref<MeshData> ExplicitRenderer::CreateMeshData()
    {
        return std::make_shared<ExplicitMeshData>();
    }

    Ref<Shader> ExplicitRenderer::CreateShader()
    {
        return std::make_shared<ExplicitShader>();
    }

    Ref<Texture2D> ExplicitRenderer::CreateTexture2D()
    {
        return std::make_shared<ExplicitTexture2D>();
    }

    Ref<Texture2DArray> ExplicitRenderer::CreateTexture2DArray()
    {
        return std::make_shared<ExplicitTexture2DArray>();
    }

    Ref<RenderTarget> ExplicitRenderer::CreateRenderTarget()
    {
        return std::make_shared<ExplicitRenderTarget>(m_driver);
    }

    void ExplicitRenderer::EnqueueDeletionFunc(const std::function<void()>& func)
    {
        m_deletionQueues[m_currentFrame].push_back(func);
    }

    void ExplicitRenderer::StartFrame()
    {
        m_currentFrame = Application::Get().GetCurrentFrame();

        m_driver.WaitForFences(1, &m_inFlightFences[m_currentFrame]);

        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        m_driver.ResetCommandBuffer(buffer);
        m_driver.BeginCommandBuffer(buffer);

        m_driver.CmdBeginRendering(buffer);

        // TODO: camera uniforms
    }

    void ExplicitRenderer::RenderFrame(RenderTargetID& target, std::span<Scene*> scenes)
    {
        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        // TODO: acquire image
        // if (m_ShouldRecreateSwapchain)
        //     RecreateSwapchain();
        // // The swapchain has one more than the minimum images, so the index
        // // might not be the same as m_CurrentFrame
        // const VkResult acquireResult = vkAcquireNextImageKHR(
        //     m_Device, m_Swapchain->Get(),
        //     UINT64_MAX,
        //     m_ImageAvailableSemaphores[m_CurrentFrame],
        //     VK_NULL_HANDLE,
        //     &m_CurrentImageIndex
        // );
        // if (acquireResult != VK_SUCCESS)
        // {
        //     m_ShouldRecreateSwapchain = true;
        //     return false;
        // }

        m_driver.CmdSetViewport(buffer);
        m_driver.CmdSetScissor(buffer);
    }

    void ExplicitRenderer::EndFrame()
    {

    }

    void ExplicitRenderer::ProcessDeletionQueue()
    {
        for (auto& func : m_deletionQueues[m_currentFrame])
            func();

        m_deletionQueues[m_currentFrame].clear();
    }

    void ExplicitRenderer::BeginScene()
    {
        // TODO: scene uniforms

        // const auto& vkUniforms = static_cast<DynamicUniforms&>(scene.GetUniforms());
        // m_Sets[1] = vkUniforms.GetDescriptorSet();
    }

    void ExplicitRenderer::UseMaterial()
    {
    //     const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
    //     const le::DynamicUniforms& dynamicUniforms = material.GetUniforms();
    //     const auto& vkUniforms = static_cast<const DynamicUniforms&>(
    //         dynamicUniforms);
    //
    //     m_Sets[2] = vkUniforms.GetDescriptorSet();
    //     m_HaveSetsChanged = true;
    //
    //     if (material.GetShader() == m_currentShaderID)
    //         return;
    //
    //     const le::Pipeline& pipeline = shader->GetPipeline();
    //     const auto& vkPipeline = static_cast<const Pipeline&>(pipeline);
    //
    //     VkCullModeFlags cullMode;
    //     switch (shader->GetCullMode())
    //     {
    //         case Shader::CullMode::BACK: cullMode = VK_CULL_MODE_BACK_BIT; break;
    //         case Shader::CullMode::FRONT: cullMode = VK_CULL_MODE_FRONT_BIT; break;
    //         default: cullMode = VK_CULL_MODE_NONE;
    //     }
    //
    //     vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //         vkPipeline.Get());
    //     vkCmdSetCullMode(buffer, cullMode);
    //
    //     m_currentPipelineLayout = vkPipeline.GetPipelineLayout();
    //     m_currentShaderID = material.GetShader();

        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        m_driver.CmdBindPipeline(buffer);
        m_driver.CmdSetCullMode(buffer);
    }

    void ExplicitRenderer::DrawMesh()
    {
    //     auto& vertexBuffer = static_cast<Buffer&>(meshData->GetVertexBuffer());
    //     auto& indexBuffer  = static_cast<Buffer&>(meshData->GetIndexBuffer());
    //
    //     if (!vertexBuffer.GetDesc().buffer || !indexBuffer.GetDesc().buffer)
    //         return;
    //
    //     const VkCommandBuffer buffer = m_CommandBuffers[m_CurrentFrame];
    //
    //     Pipeline::ObjectTransform objectTransform;
    //     objectTransform.transform = transform.transformMat;
    //
    //     vkCmdPushConstants(buffer, m_currentPipelineLayout,
    //         VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(objectTransform),
    //         &objectTransform);
    //
    //     if (m_HaveSetsChanged)
    //     {
    //         vkCmdBindDescriptorSets(
    //             buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //             m_currentPipelineLayout,
    //             0, std::size(m_Sets),
    //             m_Sets,
    //             0, nullptr
    //         );
    //         m_HaveSetsChanged = false;
    //     }
    //
    //     const VkBuffer vBuffers[] = { vertexBuffer.GetDesc().buffer };
    //     constexpr VkDeviceSize offsets[] = { 0 };
    //     vkCmdBindVertexBuffers(buffer, 0, 1, vBuffers, offsets);
    //
    //     vkCmdBindIndexBuffer(
    //         buffer,
    //         indexBuffer.GetDesc().buffer,
    //         0,
    //         VK_INDEX_TYPE_UINT32
    //     );
    //
    //     vkCmdDrawIndexed(buffer, meshData->GetIndexCount(),
    //         1, 0, 0, 0);

        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        m_driver.CmdPushConstants(buffer);

        if (m_haveSetsChanged)
        {
            m_driver.CmdBindDescriptorSets(buffer);
            m_haveSetsChanged = false;
        }

        m_driver.CmdBindVertexBuffers(buffer);
        m_driver.CmdBindIndexBuffer(buffer);

        m_driver.CmdDrawIndexed(buffer);
    }

    void ExplicitRenderer::CreateCommandBuffers() const
    {
        m_driver.AllocateCommandBuffers(m_gfxPool);
    }

    void ExplicitRenderer::CreateSyncObjects()
    {
        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; ++i)
        {
            m_inFlightFences[i] = m_driver.CreateFence();
            m_renderFinishedSemaphores[i] = m_driver.CreateSemaphore();
        }
    }
}
