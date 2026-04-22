#pragma once

#include <LE/TetherBindings.hpp>
#include <LE/Graphics/WindowManager.hpp>

namespace le
{
    class TetherWindowManager : public WindowManager
    {
    public:
        TetherWindowManager(GraphicsResources& resources, std::string_view title, int width, int height);

        void SetVisible(bool visible) const override;
        void AddResizeCallback(const ResizeCallback& callback) override;
        void SetActiveCamera(UID cameraID) const override;
        bool IsCloseRequested() const override;

        [[nodiscard]] Window& GetWindow() override;
        [[nodiscard]] RenderTarget& GetRenderTarget() const override;
    private:
        class ResizeHandler : public Tether::Events::EventHandler
        {
        public:
            explicit ResizeHandler(TetherWindowManager& impl)
                :
                m_impl(impl)
            {}

            void OnWindowResize(const Tether::Events::WindowResizeEvent& event) override
            {
                for (ResizeCallback& callback : m_impl.m_resizeCallbacks)
                    callback(static_cast<int>(event.GetNewWidth()), static_cast<int>(event.GetNewHeight()));
            }

            TetherWindowManager& m_impl;
        };

        static std::wstring GetWideTitle(std::string_view title);

        Window m_window;
        Scope<RenderTarget> m_renderTarget;

        ResizeHandler m_resizeHandler = ResizeHandler(*this);

        std::vector<ResizeCallback> m_resizeCallbacks;
    };
}
