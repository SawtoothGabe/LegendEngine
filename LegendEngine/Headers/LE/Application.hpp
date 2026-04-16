#pragma once

#include <memory>
#include <LE/TetherBindings.hpp>
#include <LE/Common/Assert.hpp>
#include <LE/Common/Defs.hpp>
#include <LE/Events/EventBus.hpp>
#include <LE/Graphics/Explicit/ExplicitDriver.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/IO/Logger.hpp>

namespace le
{
    enum class GraphicsAPI;

    class Application final
    {
    public:
        static constexpr size_t FRAMES_IN_FLIGHT = 2;

        Application(
            Scope<ExplicitDriver> driver,
            std::string_view applicationName,
            int width = 1280, int height = 720);

        template<typename... Args>
        static void Create(GraphicsAPI api, std::string_view applicationName, Args&&... args)
        {
            LE_INFO("Creating application");
            LE_ASSERT(!m_Instance, "Application already exists");
            m_Instance = std::make_unique<Application>(ExplicitDriver::Create(api, applicationName),
                applicationName, args...);
        }
        ~Application();

        LE_NO_COPY(Application);

        void SetActiveScene(Scene& scene);
        void ClearActiveScene();

        Tether::Window& GetWindow() const;
        GraphicsContext& GetGraphicsContext();
        EventBus& GetEventBus();
        Scene& GetGlobalScene();
        Scene* GetActiveScene() const;
        size_t GetCurrentFrame() const;

        // Must be called on the main thread
        void AdvanceFrame(float delta = 1.0f);

        static void Run();
        static void Destroy();

        static bool HasConstructed();
        static Application& Get();
    private:
        void RunInstance();

        void Update(float delta, bool updateWindow = true);
        void Render(float delta);

        static void RecalculateTransforms(Scene& scene);

        EventBus m_EventBus;

#ifndef LE_HEADLESS
        class ResizeHandler : public Tether::Events::EventHandler
        {
        public:
            explicit ResizeHandler(Application& app);

            void OnWindowResize(const Tether::Events::WindowResizeEvent& event) override;

            Application& m_Application;
        };
        ResizeHandler m_ResizeHandler;
        Scope<Window> m_Window = nullptr;
#endif

        GraphicsContext m_graphicsContext;
        RenderTargetID m_renderTarget;

        Scene m_GlobalScene;
        Scene* m_pActiveScene = nullptr;

        bool m_Headless = false;

        size_t m_currentFrame = 0;

        static Scope<Application> m_Instance;
    };
}
