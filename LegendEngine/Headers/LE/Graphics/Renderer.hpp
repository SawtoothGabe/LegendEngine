#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Components/Camera.hpp>
#include <LE/Components/Mesh.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Math/Types.hpp>
#include <LE/Resources/Material.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    class GraphicsResources;
    class Application;
    class GraphicsContext;
    class Renderer
    {
    public:
        explicit Renderer(RenderTarget& renderTarget, GraphicsContext& context, GraphicsResources& resources);
        virtual ~Renderer() = 0;
        LE_NO_COPY(Renderer);

        void RenderFrame(std::span<Scene*> scenes);

        void SetClearColor(const Vector4f& color);
        virtual void SetVSyncEnabled(bool vsync) {}

        virtual void NotifyWindowResized() {}

        [[nodiscard]] RenderTarget& GetRenderTarget() const;
    protected:
        virtual bool StartFrame() = 0;
        virtual void BeginScene(Scene& scene) = 0;
        virtual void UseMaterial(const Material& material, Ref<Shader> shader) = 0;
        virtual void DrawMesh(const Mesh& mesh, const Transform& transform, Ref<MeshData> data) = 0;
        virtual void EndFrame() = 0;

        RenderTarget& m_RenderTarget;
        Vector4f m_ClearColor = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

        Scope<Buffer> m_cameraUniformBuffer;
        Scope<DynamicUniforms> m_cameraUniforms;
    private:
        void RenderScene(Scene& scene);
        void UpdateCamera(Scene& scene, UID cameraID) const;
        void UpdateCameraUniforms(const Camera& camera) const;

        GraphicsResources& m_resources;
    };
}
