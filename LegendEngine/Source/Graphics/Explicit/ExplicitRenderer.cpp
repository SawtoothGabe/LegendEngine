#include <LE/Application.hpp>
#include <LE/Components/Camera.hpp>
#include <LE/Components/Mesh.hpp>
#include <LE/Components/Transform.hpp>
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

        m_defaultMaterial = Material::Create();

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

    MeshID ExplicitRenderer::CreateMesh(const std::span<MeshData::Vertex3> vertices, const std::span<uint32_t> indices,
        const MeshData::UpdateFrequency frequency)
    {
        return MeshID(new ExplicitMesh(*this, vertices, indices, frequency));
    }

    MeshID ExplicitRenderer::CreateMesh(const size_t initialVertexCount, const size_t initialIndexCount,
        const MeshData::UpdateFrequency frequency)
    {
        return MeshID(new ExplicitMesh(*this, initialVertexCount, initialIndexCount, frequency));
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

    Texture2DID ExplicitRenderer::CreateTexture2D(const TextureData& loader)
    {
        return Texture2DID(new ExplicitTexture2D(*this, loader));
    }

    Texture2DArrayID ExplicitRenderer::CreateTexture2DArray(const size_t width, const size_t height,
        const uint8_t channels, const std::span<TextureData*>& textureData)
    {
        return Texture2DArrayID(new ExplicitTexture2DArray(*this, width, height, channels, textureData));
    }

    PoolManagerID& ExplicitRenderer::GetMaterialPoolManager() const
    {

    }

    CommandPoolID ExplicitRenderer::GetGraphicsPool() const
    {
        return m_gfxPool;
    }

    CommandPoolID ExplicitRenderer::GetTransferPool() const {}
    QueueID ExplicitRenderer::GetGraphicsQueue() const {}
    QueueID ExplicitRenderer::GetTransferQueue() const {}
    std::mutex& ExplicitRenderer::GetGraphicsMutex() const {}
    std::mutex& ExplicitRenderer::GetTransferMutex() const {}

    ImageID ExplicitRenderer::GetTexture2DImage(const Texture2DID texture)
    {
        return reinterpret_cast<ExplicitTexture2D*>(texture.id)->GetImage();
    }

    ImageViewID ExplicitRenderer::GetTexture2DImageView(const Texture2DID texture)
    {
        return reinterpret_cast<ExplicitTexture2D*>(texture.id)->GetImageView();
    }

    ImageID ExplicitRenderer::GetTexture2DArrayImage(const Texture2DArrayID texture)
    {
        return reinterpret_cast<ExplicitTexture2DArray*>(texture.id)->GetImage();
    }

    ImageViewID ExplicitRenderer::GetTexture2DArrayImageView(const Texture2DArrayID texture)
    {
        return reinterpret_cast<ExplicitTexture2DArray*>(texture.id)->GetImageView();
    }

    void ExplicitRenderer::CreatePipelineLayout()
    {
        PushConstantRange ranges[] = {
            {
                .size = sizeof(Transform),
                .offset = 0,
                .stage = ShaderStageFlagBits::VERTEX
            }
        };



        m_pipelineLayout = m_driver->CreatePipelineLayout(ranges, layouts);
    }

    RenderTargetID ExplicitRenderer::CreateRenderTarget(Window& window)
    {
        constexpr auto color = Format::B8G8R8A8_SRGB;
        return RenderTargetID(new ExplicitRenderTarget(*this, color, m_depthFormat, window));
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
        EnqueueDeletionFunc([this, id] { m_driver->DestroyPipeline(PipelineID(id.id)); });
    }

    void ExplicitRenderer::DestroyTexture2D(Texture2DID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitTexture2D*>(id.id); });
    }

    void ExplicitRenderer::DestroyTexture2DArray(Texture2DArrayID id)
    {
        EnqueueDeletionFunc([id] { delete reinterpret_cast<ExplicitTexture2DArray*>(id.id); });
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
    }

    void ExplicitRenderer::RenderFrame(RenderTargetID& target, std::span<Scene*> scenes)
    {
        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];
        auto& explicitTarget = reinterpret_cast<ExplicitRenderTarget&>(target.id);

        const UID cameraID = explicitTarget.GetActiveCameraID();
        if (cameraID == 0)
        {
            LE_WARN("Camera was not set. Skipping render target.");
            return;
        }

        explicitTarget.StartRendering(buffer, m_currentFrame);
        {
            Scene* sceneWithCamera = nullptr;
            for (Scene* scene : scenes)
            {
                if (!scene || !scene->HasEntity(cameraID))
                    continue;

                sceneWithCamera = scene;
            }

            if (!sceneWithCamera)
            {
                LE_WARN("Camera not found. Skipping render target.");
                explicitTarget.EndRendering(buffer);
                return;
            }

            UseMaterial(m_defaultMaterial);
            UpdateCamera(*sceneWithCamera, cameraID);
            explicitTarget.UpdateCameraUniforms(sceneWithCamera->GetComponentData<Camera>(cameraID));

            for (Scene* pScene : scenes)
                if (pScene)
                    RenderScene(*pScene);
        }
        explicitTarget.EndRendering(buffer);
        m_targetsRendered.push_back(target);
    }

    void ExplicitRenderer::EndFrame()
    {
        for (RenderTargetID& target : m_targetsRendered)
            reinterpret_cast<ExplicitRenderTarget&>(target.id).EndFrame(m_renderFinishedSemaphores[m_currentFrame]);

        m_targetsRendered.clear();
    }

    void ExplicitRenderer::ProcessDeletionQueue()
    {
        for (auto& func : m_deletionQueues[m_currentFrame])
            func();

        m_deletionQueues[m_currentFrame].clear();
    }

    void ExplicitRenderer::BeginScene(Scene& scene)
    {
        // TODO: scene uniforms

        // const auto& vkUniforms = static_cast<DynamicUniforms&>(scene.GetUniforms());
        // m_Sets[1] = vkUniforms.GetDescriptorSet();
        m_haveSetsChanged = true;
    }

    void ExplicitRenderer::UseMaterial(const Ref<Material>& material)
    {
        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        auto& explicitMaterial = *reinterpret_cast<ExplicitMaterial*>(material->GetHandle().id);
        m_sets[2] = explicitMaterial.GetSet(m_currentFrame);
        m_haveSetsChanged = true;

        const Ref<Shader> shader = material->GetShader();
        if (shader == m_currentShader)
            return;

        const auto pipeline = PipelineID(shader->GetHandle().id);

        m_driver->CmdBindPipeline(buffer, PipelineBindPoint::GRAPHICS, pipeline);
        m_driver->CmdSetCullMode(buffer, shader->GetCullMode());

        m_currentShader = shader;
    }

    void ExplicitRenderer::RenderScene(Scene& scene)
    {
        // scene.UpdateUniforms();
        BeginScene(scene);

        Ref<Material> lastMaterial = nullptr;
        scene.QueryComponents<Mesh, Transform>(
        [&](const Mesh& mesh, const Transform& transform)
        {
            if (!mesh.enabled)
                return;

            mesh.material->UpdateUniforms();

            if (mesh.material != lastMaterial)
            {
                UseMaterial(mesh.material);
                lastMaterial = mesh.material;
            }

            DrawMesh(mesh, transform);
        });
    }

    void ExplicitRenderer::UpdateCamera(Scene& scene, const UID cameraID)
    {
        scene.QueryEntityComponents<Camera, Transform>(cameraID, [&](Camera& camera, Transform& transform)
        {
            if (camera.IsCameraDirty())
            {
                camera.CalculateProjectionMatrix();
            }

            if (transform.dirty)
            {
                camera.CalculateViewMatrix(transform);
                transform.dirty = false;
            }
        });
    }

    void ExplicitRenderer::DrawMesh(const Mesh& mesh, const Transform& transform)
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

        m_driver->CmdPushConstants(buffer, m_pipelineLayout, ShaderStageFlagBits::VERTEX,
            0, sizeof(Transform), &transform);

        if (m_haveSetsChanged)
        {
            m_driver->CmdBindDescriptorSets(buffer, PipelineBindPoint::GRAPHICS,
                m_pipelineLayout, 0, m_sets);
            m_haveSetsChanged = false;
        }

        m_driver->CmdBindVertexBuffers(buffer, 0, );
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
