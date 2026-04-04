#pragma once

#include <memory>
#include <LE/TetherBindings.hpp>
#include <LE/Common/Assert.hpp>
#include <LE/Common/Defs.hpp>
#include <LE/Events/EventBus.hpp>
#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Graphics/GraphicsResources.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/IO/Logger.hpp>

namespace le
{
    enum class GraphicsAPI;

    class Application final
    {
    public:
        static constexpr size_t FRAMES_IN_FLIGHT = 2;

#ifndef LE_HEADLESS
        // Creates the Application with a WindowRenderTarget
        Application(
            Scope<GraphicsContext> graphicsContext,
            std::string_view applicationName,
            int width, int height);

        [[nodiscard]] Renderer& GetRenderer() const;
        Tether::Window& GetWindow() const;

        template<typename... Args>
        static void Create(GraphicsAPI api, std::string_view applicationName, Args&&... args)
        {
            LE_ASSERT(!m_Instance, "Application already exists");
            m_Instance = std::make_unique<Application>(GraphicsContext::Create(api, applicationName),
                applicationName, args...);
            m_Instance->CreateResources();
        }
#else
        Application(GraphicsContext& ctx); // Headless application constructor

        template<typename... Args>
        static void Create(Args&&... args)
        {
            LE_ASSERT(!m_Instance, "Application already exists");
            m_Instance = std::make_unique<Application>(args...);
        }
#endif
        ~Application();

        LE_NO_COPY(Application);

        void SetActiveScene(Scene& scene);
        void ClearActiveScene();

        GraphicsContext& GetGraphicsContext() const;
        GraphicsResources& GetGraphicsResources();
        RenderTarget& GetRenderTarget() const;
        EventBus& GetEventBus();
        ResourceManager& GetResourceManager();
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
        void CreateResources();

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

        RenderTarget& CreateRenderTarget(int width, int height,
            std::string_view applicationName);
        Renderer& CreateRenderer();

        Scope<GraphicsContext> m_ManagedGraphicsContext = nullptr;
#endif
        std::optional<GraphicsResources> m_GraphicsResources;

        Scene m_GlobalScene;
        Scene* m_pActiveScene = nullptr;
#ifndef LE_HEADLESS
        Scope<RenderTarget> m_WindowRenderTarget = nullptr;
        Scope<Renderer> m_ManagedRenderer = nullptr;

        Scope<Window> m_Window = nullptr;
#endif
        GraphicsContext& m_GraphicsContext;
#ifndef LE_HEADLESS
        RenderTarget& m_RenderTarget;
        Renderer& m_Renderer;
#endif

        bool m_Headless = false;

        size_t m_currentFrame = 0;

        static Scope<Application> m_Instance;
    };
}
