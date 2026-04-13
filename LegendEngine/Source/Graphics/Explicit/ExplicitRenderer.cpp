#include <LE/Application.hpp>
#include <LE/Graphics/Explicit/ExplicitMaterial.hpp>
#include <LE/Graphics/Explicit/ExplicitMesh.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderTarget.hpp>
#include <LE/Graphics/Explicit/ExplicitTexture2D.hpp>
#include <LE/Graphics/Explicit/ExplicitTexture2DArray.hpp>

namespace le
{
    ExplicitRenderer::ExplicitRenderer(Scope<ExplicitDriver> driver)
        :
        m_driver(std::move(driver))
    {
        LE_INFO("Creating ExplicitRenderer");

        m_gfxPool = m_driver->CreateCommandPool(QueueFamily::GRAPHICS);
        m_depthFormat = m_driver->FindDepthFormat();

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

        m_driver->WaitIdle();

        for (auto& queue : m_deletionQueues)
            for (auto& deletionFunc : queue)
                deletionFunc();

        m_driver->FreeCommandBuffers(m_gfxPool, m_commandBuffers.size(), m_commandBuffers.data());

        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; ++i)
        {
            m_driver->DestroyFence(m_inFlightFences[i]);
            m_driver->DestroySemaphore(m_renderFinishedSemaphores[i]);
        }

        m_driver->DestroyCommandPool(m_gfxPool);
        m_driver->DestroyPipelineLayout(m_pipelineLayout);

        LE_INFO("Destroyed ExplicitRenderer");
    }

    MaterialID ExplicitRenderer::CreateMaterial()
    {
        return MaterialID(new ExplicitMaterial(*this));
    }

    MeshID ExplicitRenderer::CreateMesh()
    {
        return MeshID(new ExplicitMesh());
    }

    ShaderID ExplicitRenderer::CreateShader(const sh::ShaderInfo& shaderInfo)
    {
        std::vector<VertexBinding> bindings;
        std::vector<VertexAttribute> attributes;

        // Vertex3
        {
            bindings.push_back({
                .binding = 0,
                .stride = sizeof(MeshData::Vertex3),
                .inputRate = InputRate::VERTEX
            });

            attributes.push_back({
                .location = 0,
                .binding = 0,
                .offset = offsetof(MeshData::Vertex3, position),
                .format = Format::R32G32B32_SFLOAT,
            });

            attributes.push_back({
                .location = 1,
                .binding = 0,
                .offset = offsetof(MeshData::Vertex3, texcoord),
                .format = Format::R32G32_SFLOAT,
            });
        }

        PipelineInfo info;
        info.colorAttachmentFormats = std::span(&COLOR_FORMAT, 1);
        info.depthFormat = m_depthFormat;
        info.layout = m_pipelineLayout;
        info.shaderInfo = shaderInfo;
        info.vertexBindings = bindings;
        info.vertexAttributes = attributes;

        return ShaderID(m_driver->CreatePipeline(info).id);
    }

    Texture2DID ExplicitRenderer::CreateTexture2D()
    {
        return Texture2DID(new ExplicitTexture2D());
    }

    Texture2DArrayID ExplicitRenderer::CreateTexture2DArray()
    {
        return Texture2DArrayID(new ExplicitTexture2DArray());
    }

    RenderTargetID ExplicitRenderer::CreateRenderTarget(Window& window)
    {
        return RenderTargetID(new ExplicitRenderTarget(*m_driver, window));
    }

    void ExplicitRenderer::DestroyMaterial(MaterialID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitMaterial*>(id.id); });
    }

    void ExplicitRenderer::DestroyMesh(MeshID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitMesh*>(id.id); });
    }

    void ExplicitRenderer::DestroyShader(ShaderID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitShader*>(id.id); });
    }

    void ExplicitRenderer::DestroyTexture2D(Texture2DID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitTexture2D*>(id.id); });
    }

    void ExplicitRenderer::DestroyTexture2DArray(Texture2DArrayID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitTexture2D*>(id.id); });
    }

    void ExplicitRenderer::DestroyRenderTarget(RenderTargetID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitRenderTarget*>(id.id); });
    }

    void ExplicitRenderer::EnqueueDeletionFunc(const std::function<void()>& func)
    {
        m_deletionQueues[m_currentFrame].push_back(func);
    }

    ExplicitDriver& ExplicitRenderer::GetDriver() const
    {
        return *m_driver;
    }

    void ExplicitRenderer::StartFrame()
    {
        m_currentFrame = Application::Get().GetCurrentFrame();

        m_driver->WaitForFences(1, &m_inFlightFences[m_currentFrame]);

        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        m_driver->ResetCommandBuffer(buffer);
        m_driver->BeginCommandBuffer(buffer, false);

        m_driver->CmdBeginRendering(buffer);

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

        m_driver->CmdSetViewport(buffer);
        m_driver->CmdSetScissor(buffer);
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

        m_driver->CmdBindPipeline(buffer);
        m_driver->CmdSetCullMode(buffer);
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

        m_driver->CmdPushConstants(buffer);

        if (m_haveSetsChanged)
        {
            m_driver->CmdBindDescriptorSets(buffer);
            m_haveSetsChanged = false;
        }

        m_driver->CmdBindVertexBuffers(buffer);
        m_driver->CmdBindIndexBuffer(buffer);

        m_driver->CmdDrawIndexed(buffer);
    }

    void ExplicitRenderer::CreateCommandBuffers()
    {
        m_commandBuffers = m_driver->AllocateCommandBuffers(m_gfxPool, Application::FRAMES_IN_FLIGHT);
    }

    void ExplicitRenderer::CreateSyncObjects()
    {
        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; ++i)
        {
            m_inFlightFences[i] = m_driver->CreateFence(true);
            m_renderFinishedSemaphores[i] = m_driver->CreateSemaphore();
        }
    }
}
