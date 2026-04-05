#include <LE/Application.hpp>
#include <LE/Graphics/ExplicitRenderer.hpp>

namespace le
{
    ExplicitRenderer::ExplicitRenderer(GraphicsDriver& driver)
        :
        m_driver(driver)
    {

    }

    ExplicitRenderer::~ExplicitRenderer()
    {

    }

    MaterialHandle ExplicitRenderer::CreateMaterial() {}
    MeshHandle ExplicitRenderer::CreateMesh() {}
    ShaderHandle ExplicitRenderer::CreateShader() {}
    Texture2DHandle ExplicitRenderer::CreateTexture2D() {}
    Texture2DArrayHandle ExplicitRenderer::CreateTexture2DArray() {}
    RenderTargetHandle ExplicitRenderer::CreateRenderTarget() {}

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

    void ExplicitRenderer::RenderFrame(RenderTarget& target, std::span<Scene*> scenes)
    {
        const CommandBufferID buffer = m_commandBuffers[m_currentFrame];

        // TODO: acquire image

        m_driver.CmdSetViewport(buffer);
        m_driver.CmdSetScissor(buffer);
    }

    void ExplicitRenderer::EndFrame()
    {

    }
}
