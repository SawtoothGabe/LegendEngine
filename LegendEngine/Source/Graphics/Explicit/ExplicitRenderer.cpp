#include <LE/Application.hpp>
#include <LE/Components/Camera.hpp>
#include <LE/Components/Light.hpp>
#include <LE/Components/Mesh.hpp>
#include <LE/Components/Transform.hpp>
#include <LE/Graphics/Explicit/ExplicitMaterial.hpp>
#include <LE/Graphics/Explicit/ExplicitMesh.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderer.hpp>
#include <LE/Graphics/Explicit/ExplicitRenderTarget.hpp>
#include <LE/Graphics/Explicit/ExplicitScene.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    ExplicitRenderer::ExplicitRenderer(ExplicitDriver& driver)
        :
        m_driver(driver)
    {
        LE_INFO("Creating ExplicitRenderer");

        m_depthFormat = m_driver.FindDepthFormat();

        m_renderFinishedSemaphores.resize(Application::FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(Application::FRAMES_IN_FLIGHT);

        CreateQueues();
        CreateCommandBuffers();
        CreateSyncObjects();
        CreateDescriptorSetLayouts();
        CreatePipelineLayout();

        m_defaultMaterial = Material::Create();

        LE_INFO("Created ExplicitRenderer");
    }

    ExplicitRenderer::~ExplicitRenderer()
    {
        LE_INFO("Destroying ExplicitRenderer");

        m_driver.WaitIdle();

        m_driver.FreeCommandBuffers(m_graphicsPool, m_commandBuffers.size(), m_commandBuffers.data());

        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; ++i)
        {
            m_driver.DestroyFence(m_inFlightFences[i]);
            m_driver.DestroySemaphore(m_renderFinishedSemaphores[i]);
        }

        m_driver.DestroyCommandPool(m_graphicsPool);
        m_driver.DestroyPipelineLayout(m_pipelineLayout);

        m_driver.DestroyLayoutPoolManager(m_cameraPool);
        m_driver.DestroyLayoutPoolManager(m_scenePool);
        m_driver.DestroyLayoutPoolManager(m_materialPool);
        for (const DescriptorSetLayoutID& layout : m_descriptorSetLayouts)
            m_driver.DestroyDescriptorSetLayout(layout);

        LE_INFO("Destroyed ExplicitRenderer");
    }

    void ExplicitRenderer::StartFrame()
    {
        m_currentFrame = Application::Get().GetCurrentFrame();

        m_driver.WaitForFences(1, &m_inFlightFences[m_currentFrame]);

        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        m_driver.ResetCommandBuffer(buffer);
        m_driver.BeginCommandBuffer(buffer, false);
    }

    void ExplicitRenderer::RenderFrame(RenderTargetID& target, std::span<Scene*> scenes)
    {
        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];
        const auto& explicitTarget = reinterpret_cast<ExplicitRenderTarget&>(target.id);

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
            explicitTarget.UpdateCameraUniforms(m_currentFrame, sceneWithCamera->GetComponentData<Camera>(cameraID));

            for (Scene* pScene : scenes)
                if (pScene)
                    RenderScene(*pScene);
        }
        explicitTarget.EndRendering(buffer);
        m_targetsRendered.push_back(target);
    }

    void ExplicitRenderer::EndFrame()
    {
        std::vector<SemaphoreID> waitSemaphores;
        for (const RenderTargetID& target : m_targetsRendered)
        {
            auto& explicitTarget = *reinterpret_cast<ExplicitRenderTarget*>(target.id);
            waitSemaphores.emplace_back(explicitTarget.GetImageAvailableSemaphore(m_currentFrame));
        }

        PipelineStage stageMask[] = { PipelineStage::COLOR_ATTACHMENT_OUTPUT };
        SemaphoreID signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };

        SubmitInfo info;
        info.commandBuffer = m_commandBuffers[m_currentFrame];
        info.fence = m_inFlightFences[m_currentFrame];
        info.waitDstStageMask = stageMask;
        info.waitSemaphores = waitSemaphores;
        info.signalSemaphores = signalSemaphores;

        {
            std::scoped_lock lock(m_graphicsMutex);
            m_driver.QueueSubmit(m_graphicsQueue, info);
        }

        for (const RenderTargetID& target : m_targetsRendered)
        {
            auto& explicitTarget = *reinterpret_cast<ExplicitRenderTarget*>(target.id);
            explicitTarget.EndFrame(m_renderFinishedSemaphores[m_currentFrame]);
        }

        m_targetsRendered.clear();
    }

    PoolManagerID ExplicitRenderer::GetCameraPoolManager() const
    {
        return m_cameraPool;
    }

    PoolManagerID ExplicitRenderer::GetScenePoolManager() const
    {
        return m_scenePool;
    }

    PoolManagerID ExplicitRenderer::GetMaterialPoolManager() const
    {
        return m_materialPool;
    }

    CommandPoolID ExplicitRenderer::GetGraphicsPool() const
    {
        return m_graphicsPool;
    }

    CommandPoolID ExplicitRenderer::GetTransferPool() const
    {
        return *m_pTransferPool;
    }

    QueueID ExplicitRenderer::GetGraphicsQueue() const
    {
        return m_graphicsQueue;
    }

    QueueID ExplicitRenderer::GetTransferQueue() const
    {
        return *m_pTransferQueue;
    }

    std::mutex& ExplicitRenderer::GetGraphicsMutex()
    {
        return m_graphicsMutex;
    }

    std::mutex& ExplicitRenderer::GetTransferMutex() const
    {
        return *m_pTransferMutex;
    }

    ExplicitDriver& ExplicitRenderer::GetDriver() const
    {
        return m_driver;
    }

    void ExplicitRenderer::CreateQueues()
    {
        m_graphicsPool = m_driver.CreateCommandPool(QueueFamily::GRAPHICS);
        m_graphicsQueue = m_driver.GetQueue(QueueFamily::GRAPHICS);
        m_pTransferPool = &m_graphicsPool;
        m_pTransferQueue = &m_graphicsQueue;
        m_pTransferMutex = &m_graphicsMutex;

        if (m_driver.HasTransferQueue())
        {
            m_transferPool = m_driver.CreateCommandPool(QueueFamily::TRANSFER);
            m_transferQueue = m_driver.GetQueue(QueueFamily::TRANSFER);
            m_pTransferPool = &m_transferPool;
            m_pTransferQueue = &m_transferQueue;
            m_pTransferMutex = &m_transferMutex;
        }
    }

    void ExplicitRenderer::CreateCommandBuffers()
    {
        m_commandBuffers = m_driver.AllocateCommandBuffers(m_graphicsPool, Application::FRAMES_IN_FLIGHT);
    }

    void ExplicitRenderer::CreateSyncObjects()
    {
        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; ++i)
        {
            m_inFlightFences[i] = m_driver.CreateFence(true);
            m_renderFinishedSemaphores[i] = m_driver.CreateSemaphore();
        }
    }

    void ExplicitRenderer::CreateDescriptorSetLayouts()
    {
        // Camera
        {
            DescriptorSetLayoutBinding bindings[] =
            {
                DescriptorSetLayoutBinding {
                    .binding = 0,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stageFlags = ShaderStageFlagBits::VERTEX
                }
            };

            DescriptorSetLayoutID layout = m_driver.CreateDescriptorSetLayout(bindings);
            m_descriptorSetLayouts.emplace_back(layout);
            m_scenePool = m_driver.CreateLayoutPoolManager(layout);
        }

        // Scene
        {
            DescriptorSetLayoutBinding bindings[] =
            {
                DescriptorSetLayoutBinding {
                    .binding = 0,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stageFlags = ShaderStageFlagBits::FRAGMENT
                },
                DescriptorSetLayoutBinding {
                    .binding = 1,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::STORAGE_BUFFER,
                    .stageFlags = ShaderStageFlagBits::FRAGMENT
                }
            };

            DescriptorSetLayoutID layout = m_driver.CreateDescriptorSetLayout(bindings);
            m_descriptorSetLayouts.emplace_back(layout);
            m_scenePool = m_driver.CreateLayoutPoolManager(layout);
        }

        // Material
        {
            DescriptorSetLayoutBinding bindings[] =
            {
                DescriptorSetLayoutBinding {
                    .binding = 0,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .stageFlags = ShaderStageFlagBits::FRAGMENT
                },
                DescriptorSetLayoutBinding {
                    .binding = 1,
                    .descriptorCount = 1,
                    .descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
                    .stageFlags = ShaderStageFlagBits::FRAGMENT
                }
            };

            DescriptorSetLayoutID layout = m_driver.CreateDescriptorSetLayout(bindings);
            m_descriptorSetLayouts.emplace_back(layout);
            m_materialPool = m_driver.CreateLayoutPoolManager(layout);
        }
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

        m_pipelineLayout = m_driver.CreatePipelineLayout(ranges, m_descriptorSetLayouts);
    }

    void ExplicitRenderer::UseMaterial(const Ref<Material>& material)
    {
        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        const auto& explicitMaterial = *reinterpret_cast<ExplicitMaterial*>(material->GetHandle().id);
        m_sets[2] = explicitMaterial.GetSet(m_currentFrame);
        m_haveSetsChanged = true;

        const Ref<Shader> shader = material->GetShader();
        if (shader == m_currentShader)
            return;

        const auto pipeline = PipelineID(shader->GetHandle().id);

        m_driver.CmdBindPipeline(buffer, PipelineBindPoint::GRAPHICS, pipeline);
        m_driver.CmdSetCullMode(buffer, shader->GetCullMode());

        m_currentShader = shader;
    }

    void ExplicitRenderer::RenderScene(Scene& scene)
    {
        UpdateSceneUniforms(scene);
        BeginScene(scene);

        Ref<Material> lastMaterial = nullptr;
        scene.QueryComponents<Mesh, Transform>(
            [&](const Mesh& mesh, const Transform& transform)
            {
                if (!mesh.enabled)
                    return;

                ExplicitMaterial& mat = *reinterpret_cast<ExplicitMaterial*>(mesh.material->GetHandle().id);
                mat.UpdateUniforms(m_currentFrame);

                if (mesh.material != lastMaterial)
                {
                    UseMaterial(mesh.material);
                    lastMaterial = mesh.material;
                }

                DrawMesh(mesh, transform);
            });
    }

    void ExplicitRenderer::BeginScene(const Scene& scene)
    {
        const auto& explicitScene = *reinterpret_cast<ExplicitScene*>(scene.GetHandle().id);

        m_sets[1] = explicitScene.GetSet(m_currentFrame);
        m_haveSetsChanged = true;
    }

    void ExplicitRenderer::UpdateSceneUniforms(Scene& scene)
    {
        size_t count = 0;
        scene.QueryComponents<Transform, Light>([&count](const Transform&, Light&)
        {
            count++;
        });

        auto& explicitScene = *reinterpret_cast<ExplicitScene*>(scene.GetHandle().id);
        explicitScene.SetLightCount(count);

        size_t index = 0;
        scene.QueryComponents<Transform, Light>([&](const Transform& transform, Light& light)
        {
            const SceneLightData lightData = {
                .position = Vector4f(transform.GetPosition(), 1.0f),
                .color = light.color,
                .type = static_cast<uint32_t>(light.type),
            };

            explicitScene.UpdateLightData(index++, lightData);
        });

        explicitScene.UpdateUniforms();
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
        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];
        const ExplicitMesh& explicitMesh = *reinterpret_cast<ExplicitMesh*>(mesh.data->GetHandle().id);
        const BufferID vertex = explicitMesh.GetVertexBuffer().GetDesc(m_currentFrame).buffer;
        const BufferID index = explicitMesh.GetIndexBuffer().GetDesc(m_currentFrame).buffer;

        if (!vertex || !index)
            return;

        m_driver.CmdPushConstants(buffer, m_pipelineLayout, ShaderStageFlagBits::VERTEX,
                                   0, sizeof(Transform), &transform);

        if (m_haveSetsChanged)
        {
            m_driver.CmdBindDescriptorSets(buffer, PipelineBindPoint::GRAPHICS,
                                            m_pipelineLayout, 0, m_sets);
            m_haveSetsChanged = false;
        }

        BufferID buffers[] = { vertex };
        m_driver.CmdBindVertexBuffers(buffer, 0, buffers);
        m_driver.CmdBindIndexBuffer(buffer, index, 0);

        m_driver.CmdDrawIndexed(buffer, explicitMesh.GetIndexCount(), 1, 0, 0, 0);
    }
}
