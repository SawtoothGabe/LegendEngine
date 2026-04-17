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

    Application::Application(
        Scope<GraphicsDriver> driver,
        const std::string_view applicationName,
        const int width, const int height)
        :
        m_graphicsContext(m_EventBus, std::move(driver)),
        m_GlobalScene(m_graphicsContext.GetResources())
    {
        m_windowManager = WindowManager::Create(m_graphicsContext.GetResources(), applicationName, width, height);
        m_windowManager->AddResizeCallback([this](const int newWidth, const int newHeight)
        {
            const auto floatWidth = static_cast<float>(newWidth);
            const auto floatHeight = static_cast<float>(newHeight);
            const float aspect = floatWidth / floatHeight;

            m_GlobalScene.QueryComponents<Camera>(
                [aspect](Camera& camera)
                {
                    camera.SetAspectRatio(aspect);
                });

            if (!m_pActiveScene)
                return;

            m_pActiveScene->QueryComponents<Camera>(
                [aspect](Camera& camera)
                {
                    camera.SetAspectRatio(aspect);
                });
        });

        LE_INFO("Application created");
    }

    Application::~Application()
    {
        LE_ASSERT(m_destroyed, "Application::Destroy must be called before the program exits");
    }

    void Application::SetActiveScene(Scene& scene)
    {
        m_pActiveScene = &scene;
    }

    void Application::ClearActiveScene()
    {
        m_pActiveScene = nullptr;
    }

    WindowManager& Application::GetWindowManager() const
    {
        return *m_windowManager;
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

    size_t Application::GetCurrentFrame() const
    {
        return m_currentFrame;
    }

    void Application::AdvanceFrame(const float delta)
    {
        Update(delta);
        Render(delta);
    }

    void Application::Run()
    {
        LE_ASSERT(m_Instance, "Run was called before Application::Create");

        m_Instance->RunInstance();
    }

    void Application::Destroy()
    {
        if (!m_Instance)
            return;

        LE_INFO("Destroying application");

        m_Instance->m_destroyed = true;
        m_Instance->m_windowManager->SetVisible(false);
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

    void Application::RunInstance()
    {
        m_windowManager->SetVisible(true);

        Stopwatch deltaTimer;
        while (!m_windowManager->IsCloseRequested())
        {
            const float delta = deltaTimer.GetElapsedMillis() / 1000.0f;
            deltaTimer.Set();

            AdvanceFrame(delta);
        }
    }

    void Application::Update(const float delta, const bool updateWindow)
    {
        if (updateWindow)
            WindowManager::PollEvents();

        RecalculateTransforms(m_GlobalScene);
        m_GlobalScene.ProcessEntityChanges();
        if (m_pActiveScene)
        {
            RecalculateTransforms(*m_pActiveScene);
            m_pActiveScene->ProcessEntityChanges();
        }

        m_EventBus.DispatchEvent<UpdateEvent>(UpdateEvent(delta));
    }

    void Application::Render(const float delta)
    {
        Scene* scenes[] = {&m_GlobalScene, m_pActiveScene};
        m_graphicsContext.GetRenderer().RenderFrame(m_windowManager->GetRenderTarget(), scenes);

        m_EventBus.DispatchEvent<RenderEvent>(RenderEvent(delta));

        m_currentFrame = (m_currentFrame + 1) % FRAMES_IN_FLIGHT;
    }

    void Application::RecalculateTransforms(Scene& scene)
    {
        scene.QueryComponents<Transform>([](Transform& transform)
        {
            if (transform.dirty)
                transform.CalculateTransformMatrix();
        });
    }
}
