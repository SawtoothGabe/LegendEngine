#include <LE/Application.hpp>

#include <LE/Common/Stopwatch.hpp>
#include <LE/Components/Camera.hpp>
#include <LE/Components/Transform.hpp>
#include <LE/Events/RenderEvent.hpp>
#include <LE/Events/UpdateEvent.hpp>
#include <LE/World/Scene.hpp>

namespace le
{
    Scope<Application> Application::m_Instance = nullptr;

#ifndef LE_HEADLESS
    Application::ResizeHandler::ResizeHandler(Application& app)
        :
        m_Application(app)
    {}

    void Application::ResizeHandler::OnWindowResize(const Tether::Events::WindowResizeEvent& event)
    {
        const auto width = static_cast<float>(event.GetNewWidth());
        const auto height = static_cast<float>(event.GetNewHeight());
        const float aspect = width / height;

        m_Application.m_GlobalScene.QueryComponents<Camera>(
        [aspect](Camera& camera)
        {
            camera.SetAspectRatio(aspect);
        });

        if (!m_Application.m_pActiveScene)
            return;

        m_Application.m_pActiveScene->QueryComponents<Camera>(
        [aspect](Camera& camera)
        {
            camera.SetAspectRatio(aspect);
        });
    }

    Application::Application(
        Scope<GraphicsContext> graphicsContext,
        const std::string_view applicationName,
        const int width, const int height)
        :
        m_ResizeHandler(*this),
        m_ManagedGraphicsContext(std::move(graphicsContext)),
        m_GraphicsResources(*m_ManagedGraphicsContext),
        m_GlobalScene(*m_ManagedGraphicsContext, m_GraphicsResources.value()),
        m_GraphicsContext(*m_ManagedGraphicsContext),
        m_RenderTarget(CreateRenderTarget(width, height, applicationName)),
        m_Renderer(CreateRenderer())
    {
        LE_INFO("Application created");
    }

    Renderer& Application::GetRenderer() const
    {
        return m_Renderer;
    }

    RenderTarget& Application::GetRenderTarget() const
    {
        return m_RenderTarget;
    }

    void Application::Render(const float delta)
    {
        Scene* scenes[] = { &m_GlobalScene, m_pActiveScene };
        m_Renderer.RenderFrame(scenes);

        m_EventBus.DispatchEvent<RenderEvent>(RenderEvent(delta));

        m_currentFrame = (m_currentFrame + 1) % FRAMES_IN_FLIGHT;
    }

    void Application::RunInstance()
    {
        m_Window->SetVisible(true);

        Stopwatch deltaTimer;
        while (!m_Window->IsCloseRequested())
        {
            const float delta = deltaTimer.GetElapsedMillis() / 1000.0f;
            deltaTimer.Set();

            RenderFrame(delta);
        }
    }

    void Application::RenderFrame(const float delta)
    {
        Update(delta);
        Render(delta);
    }

    RenderTarget& Application::CreateRenderTarget(const int width, const int height,
        const std::string_view applicationName)
    {
        std::wstring title(applicationName.size(), L' ');
        std::mbstowcs(title.data(), applicationName.data(),
                      applicationName.size());

        LE_INFO("Creating window");
        m_Window = Tether::Window::Create(width, height,
                                          title, false);
        m_Window->AddEventHandler(m_ResizeHandler, Tether::Events::EventType::WINDOW_RESIZE);

        LE_INFO("Creating render target");
        return *(m_WindowRenderTarget = m_GraphicsContext.CreateWindowRenderTarget(*m_Window));
    }

    Renderer& Application::CreateRenderer()
    {
        LE_INFO("Creating renderer");
        return *(m_ManagedRenderer = m_ManagedGraphicsContext->CreateRenderer(m_RenderTarget, *m_GraphicsResources));
    }

    Tether::Window& Application::GetWindow() const
    {
        return *m_Window;
    }
#else
    Application::Application(GraphicsContext& ctx)
        :
        m_GraphicsContext(ctx)
    {
        LE_INFO("Application created");
    }
#endif

    Application::~Application()
    {
#ifndef LE_HEADLESS
        m_Window->SetVisible(false);
#endif
    }

    void Application::SetActiveScene(Scene& scene)
    {
        m_pActiveScene = &scene;
    }

    void Application::ClearActiveScene()
    {
        m_pActiveScene = nullptr;
    }

    GraphicsContext& Application::GetGraphicsContext() const
    {
        return m_GraphicsContext;
    }

    GraphicsResources& Application::GetGraphicsResources()
    {
        return *m_GraphicsResources;
    }

    EventBus& Application::GetEventBus()
    {
        return m_EventBus;
    }

    Scene& Application::GetGlobalScene()
    {
        return m_GlobalScene;
    }

    Scene* Application::GetActiveScene() const
    {
        return m_pActiveScene;
    }

    void Application::Destroy()
    {
        if (!m_Instance)
            return;

        LE_INFO("Destroying application");

#ifndef LE_HEADLESS
        m_Instance->m_Window->SetVisible(false);
#endif
        m_Instance->m_GlobalScene.Clear();

        m_Instance.reset();
    }

    bool Application::HasConstructed()
    {
        return m_Instance != nullptr;
    }

    Application& Application::Get()
    {
        if (!m_Instance)
            throw std::runtime_error(
                "Application::Get() called before it was constructed. "
                "No LegendEngine objects may be used before application creation and "
                "Application::Destroy must be called if the application wasn't created with Application::Run");

        return *m_Instance;
    }

    ResourceManager & Application::GetResourceManager()
    {
        return m_resourceManager;
    }

    void Application::Update(const float delta, const bool updateWindow)
    {
#ifndef LE_HEADLESS
        if (updateWindow)
            Tether::Application::Get().PollEvents();
#endif

        m_resourceManager.ProcessDeletedResources();

        RecalculateTransforms(m_GlobalScene);
        if (m_pActiveScene)
            RecalculateTransforms(*m_pActiveScene);

        m_EventBus.DispatchEvent<UpdateEvent>(UpdateEvent(delta));
    }

    void Application::RecalculateTransforms(Scene& scene)
    {
        scene.QueryComponents<Transform>([](Transform& transform)
        {
            if (transform.dirty)
                transform.CalculateTransformMatrix();
        });
    }

    void Application::Run()
    {
        LE_ASSERT(m_Instance, "Run was called before Application::Create");

        m_Instance->RunInstance();
    }

    size_t Application::GetCurrentFrame() const
    {
        return m_currentFrame;
    }

    void Application::CreateResources()
    {
        m_GraphicsContext.RegisterShaders(m_GraphicsResources->GetShaderManager());
        m_GraphicsResources->CreateResources(m_resourceManager);
    }
}
