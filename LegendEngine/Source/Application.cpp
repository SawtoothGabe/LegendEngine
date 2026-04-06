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
        Scope<Renderer> renderer,
        const std::string_view applicationName,
        const int width, const int height)
        :
        m_ResizeHandler(*this),
        m_graphicsContext(std::move(renderer))
    {
        std::wstring title(applicationName.size(), L' ');
        std::mbstowcs(title.data(), applicationName.data(),
                      applicationName.size());

        m_Window = Window::Create(width, height, title, false);
        m_renderTarget = m_graphicsContext.GetRenderer().CreateRenderTarget(*m_Window);

        LE_INFO("Application created");
    }

    void Application::Render(const float delta)
    {
        Scene* scenes[] = { &m_GlobalScene, m_pActiveScene };
        m_graphicsContext.GetRenderer().RenderFrame(m_renderTarget, scenes);

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

            AdvanceFrame(delta);
        }
    }

    void Application::AdvanceFrame(const float delta)
    {
        Update(delta);
        Render(delta);
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

    GraphicsContext& Application::GetGraphicsContext()
    {
        return m_graphicsContext;
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

    void Application::Update(const float delta, const bool updateWindow)
    {
#ifndef LE_HEADLESS
        if (updateWindow)
            Tether::Application::Get().PollEvents();
#endif

        RecalculateTransforms(m_GlobalScene);
        m_GlobalScene.ProcessEntityChanges();
        if (m_pActiveScene)
        {
            RecalculateTransforms(*m_pActiveScene);
            m_pActiveScene->ProcessEntityChanges();
        }

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
}
