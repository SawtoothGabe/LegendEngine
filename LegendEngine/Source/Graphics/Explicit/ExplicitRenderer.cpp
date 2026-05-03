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
    ExplicitRenderer::ExplicitRenderer(ExplicitResources& resources)
        :
        m_resources(resources),
        m_driver(resources.GetDriver())
    {
        LE_INFO("Creating ExplicitRenderer");

        m_renderFinishedSemaphores.resize(Application::FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(Application::FRAMES_IN_FLIGHT);

        CreateCommandBuffers();
        CreateSyncObjects();

        m_defaultMaterial = Material::Create(resources);

        LE_INFO("Created ExplicitRenderer");
    }

    ExplicitRenderer::~ExplicitRenderer()
    {
        LE_INFO("Destroying ExplicitRenderer");

        m_driver.WaitIdle();

        m_driver.FreeCommandBuffers(m_resources.GetGraphicsPool(), m_commandBuffers.size(), m_commandBuffers.data());

        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; ++i)
        {
            m_driver.DestroyFence(m_inFlightFences[i]);
            m_driver.DestroySemaphore(m_renderFinishedSemaphores[i]);
        }

        LE_INFO("Destroyed ExplicitRenderer");
    }

    void ExplicitRenderer::StartFrame()
    {
        m_currentFrame = Application::Get().GetCurrentFrame();

        m_driver.WaitForFences(1, &m_inFlightFences[m_currentFrame]);

        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        m_driver.ResetCommandBuffer(buffer);
        m_driver.BeginCommandBuffer(buffer, false);

        m_currentShader = nullptr;
        m_haveSetsChanged = true;
    }

    void ExplicitRenderer::RenderFrame(RenderTarget& target, const std::span<Scene*> scenes)
    {
        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];
        auto& explicitTarget = reinterpret_cast<ExplicitRenderTarget&>(target);

        const UID cameraID = explicitTarget.GetActiveCameraID();
        if (cameraID == 0)
        {
            LE_WARN("Camera was not set. Skipping render target.");
            return;
        }

        if (!explicitTarget.StartRendering(buffer, m_currentFrame))
            return;
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

            UpdateCamera(*sceneWithCamera, cameraID);
            explicitTarget.UpdateCameraUniforms(m_currentFrame, sceneWithCamera->GetComponentData<Camera>(cameraID));
            m_sets[0] = explicitTarget.GetCameraSet(m_currentFrame);

            for (Scene* pScene : scenes)
                if (pScene)
                    RenderScene(*pScene);
        }
        explicitTarget.EndRendering(buffer);
        m_targetsRendered.push_back(&target);
    }

    void ExplicitRenderer::EndFrame()
    {
        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        m_driver.EndCommandBuffer(buffer);

        m_waitSemaphores.clear();
        m_signalSemaphores.clear();
        for (RenderTarget* target : m_targetsRendered)
        {
            auto& explicitTarget = *reinterpret_cast<ExplicitRenderTarget*>(target);
            m_waitSemaphores.emplace_back(explicitTarget.GetImageAvailableSemaphore(m_currentFrame));
            m_signalSemaphores.emplace_back(explicitTarget.GetRenderFinishedSemaphore());
        }

        PipelineStage stageMask[] = { PipelineStage::COLOR_ATTACHMENT_OUTPUT };

        SubmitInfo info;
        info.commandBuffer = buffer;
        info.fence = m_inFlightFences[m_currentFrame];
        info.waitDstStageMask = stageMask;
        info.waitSemaphores = m_waitSemaphores;
        info.signalSemaphores = m_signalSemaphores;

        // The in flight fence for this frame must be reset.
        m_driver.ResetFences(1, &m_inFlightFences[m_currentFrame]);

        {
            std::scoped_lock lock(m_resources.GetGraphicsMutex());
            m_driver.QueueSubmit(m_resources.GetGraphicsQueue(), info);
        }

        for (RenderTarget* target : m_targetsRendered)
        {
            auto& explicitTarget = *reinterpret_cast<ExplicitRenderTarget*>(target);
            explicitTarget.EndFrame();
        }

        m_targetsRendered.clear();
    }

    ExplicitDriver& ExplicitRenderer::GetDriver() const
    {
        return m_driver;
    }

    void ExplicitRenderer::CreateCommandBuffers()
    {
        m_commandBuffers = m_driver.AllocateCommandBuffers(m_resources.GetGraphicsPool(), Application::FRAMES_IN_FLIGHT);
    }

    void ExplicitRenderer::CreateSyncObjects()
    {
        for (size_t i = 0; i < Application::FRAMES_IN_FLIGHT; ++i)
        {
            m_inFlightFences[i] = m_driver.CreateFence(true);
            m_renderFinishedSemaphores[i] = m_driver.CreateSemaphore();
        }
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
                if (!mesh.enabled || !mesh.data)
                    return;

                if (mesh.material)
                {
                    ExplicitMaterial& mat = *reinterpret_cast<ExplicitMaterial*>(mesh.material->GetHandle().id);
                    mat.UpdateUniforms(m_currentFrame);
                }

                if (mesh.material != lastMaterial)
                {
                    if (mesh.material)
                    {
                        UseMaterial(mesh.material);
                    }
                    else
                        UseMaterial(m_defaultMaterial);

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

    void ExplicitRenderer::UpdateSceneUniforms(Scene& scene) const
    {
        size_t count = 0;
        scene.QueryComponents<Transform, Light>([&count](const Transform&, Light&)
        {
            count++;
        });

        auto& explicitScene = *reinterpret_cast<ExplicitScene*>(scene.GetHandle().id);
        explicitScene.StartFrame(m_currentFrame, count);

        size_t index = 0;
        scene.QueryComponents<Transform, Light>([&](const Transform& transform, Light& light)
        {
            const SceneLightData lightData = {
                .position = Vector4f(transform.GetPosition(), 1.0f),
                .color = light.color,
                .type = static_cast<uint32_t>(light.type),
            };

            explicitScene.UpdateLightData(m_currentFrame, index++, lightData);
        });

        explicitScene.UpdateUniforms(m_currentFrame);
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
        const Buffer::Desc indexDesc = explicitMesh.GetIndexBuffer().GetDesc(m_currentFrame);

        if (!vertex || !indexDesc.buffer)
            return;

        m_driver.CmdPushConstants(buffer, m_resources.GetPipelineLayout(), ShaderStageFlagBits::VERTEX,
                                   0, sizeof(Matrix4x4f), &transform.transformMat);

        if (m_haveSetsChanged)
        {
            m_driver.CmdBindDescriptorSets(buffer, PipelineBindPoint::GRAPHICS,
                                            m_resources.GetPipelineLayout(), 0, m_sets);
            m_haveSetsChanged = false;
        }

        BufferID buffers[] = { vertex };
        m_driver.CmdBindVertexBuffers(buffer, 0, buffers);
        m_driver.CmdBindIndexBuffer(buffer, indexDesc.buffer, 0);

        m_driver.CmdDrawIndexed(buffer,
            indexDesc.size / sizeof(uint32_t),
            1, 0, 0, 0
        );
    }
}
