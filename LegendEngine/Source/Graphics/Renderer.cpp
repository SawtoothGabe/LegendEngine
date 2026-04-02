#include <LE/Application.hpp>
#include <LE/Graphics/Renderer.hpp>

#include <LE/Components/ActiveCamera.hpp>
#include <LE/Components/Camera.hpp>
#include <LE/Components/Mesh.hpp>
#include <LE/Components/Transform.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/IO/Logger.hpp>
#include <LE/Math/Types.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    Renderer::Renderer(RenderTarget& renderTarget, GraphicsContext& context,
        GraphicsResources& resources)
        :
        m_RenderTarget(renderTarget),
        m_resources(resources)
    {
        m_cameraUniformBuffer = context.CreatePerFrameBuffer(Buffer::UsageFlags::UNIFORM_BUFFER,
            sizeof(Camera::CameraUniforms));
        m_cameraUniforms = context.CreateDynamicUniforms(
            DynamicUniforms::UpdateFrequency::PER_FRAME, m_resources.GetCameraLayout());
    }

    Renderer::~Renderer() = default;

    void Renderer::RenderFrame(const std::span<Scene*> scenes)
    {
        if (!StartFrame())
            return;

        Scene* sceneWithCamera = nullptr;
        UID cameraID = 0;
        ActiveCamera currentCamera;
        currentCamera.priority = std::numeric_limits<float>::min();
        for (Scene* scene : scenes)
        {
            if (!scene)
                continue;

            scene->QueryArchetypes<Camera, Transform, ActiveCamera>(
            [&](const Archetype& archetype, const size_t row, Camera&, Transform&, const ActiveCamera& active)
            {
                if (active.priority >= currentCamera.priority)
                {
                    sceneWithCamera = scene;
                    cameraID = archetype.entityIDs[row];
                    currentCamera = active;
                }
            });
        }

        if (cameraID == 0)
        {
            LE_WARN("Camera was not set. Skipping frame.");
            EndFrame();
            return;
        }

        ResourceManager& manager = Application::Get().GetResourceManager();
        Material& defaultMaterial = m_resources.GetDefaultMaterial();

        UpdateCamera(*sceneWithCamera, cameraID);
        UseMaterial(defaultMaterial, manager.GetResource<Shader>(defaultMaterial.GetShader()));

        for (Scene* pScene : scenes)
            if (pScene)
                RenderScene(*pScene);

        EndFrame();
    }

    void Renderer::RenderScene(Scene& scene)
    {
        scene.UpdateUniforms();
        BeginScene(scene);

        ResourceManager& manager = Application::Get().GetResourceManager();

        Resource::ID<Material> lastMaterial = 0;
        scene.QueryComponents<Mesh, Transform>(
        [&](const Mesh& mesh, const Transform& transform)
        {
            if (!mesh.enabled)
                return;

            Ref<MeshData> resource = manager.GetResource<MeshData>(mesh.data);

            const Ref<Material> material = manager.GetResource<Material>(mesh.material);
            material->CopyUniformData();
            material->UpdateUniforms();

            if (mesh.material != lastMaterial)
            {
                UseMaterial(*material, manager.GetResource<Shader>(material->GetShader()));
                lastMaterial = mesh.material;
            }

            DrawMesh(mesh, transform, resource);
        });
    }

    void Renderer::SetClearColor(const Vector4f& color)
    {
        m_ClearColor = color;
    }

    RenderTarget& Renderer::GetRenderTarget() const
    {
        return m_RenderTarget;
    }

    void Renderer::UpdateCamera(Scene& scene, const UID cameraID) const
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

        UpdateCameraUniforms(scene.GetComponentData<Camera>(cameraID));
    }

    void Renderer::UpdateCameraUniforms(const Camera& camera) const
    {
        Camera::CameraUniforms uniforms;
        uniforms.cameraMatrix = camera.GetCameraMatrix();

        m_cameraUniformBuffer->Update(sizeof(uniforms), 0, &uniforms);
        m_cameraUniforms->UpdateUniformBuffer(*m_cameraUniformBuffer, 0);
    }
}
