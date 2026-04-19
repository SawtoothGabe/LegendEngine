#ifndef LE_HEADLESS
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <LE/Graphics/TetherWindowManager.hpp>

namespace le
{
    TetherWindowManager::TetherWindowManager(GraphicsResources& resources, const std::string_view title,
        const int width, const int height)
    {
        std::wstring wtitle(title.size(), L' ');
        std::mbstowcs(wtitle.data(), title.data(),
            wtitle.size());

        m_window = Window::Create(width, height, wtitle);
        m_renderTarget = resources.CreateRenderTarget(*m_window);

        m_window->AddEventHandler(m_resizeHandler, Tether::Events::EventType::WINDOW_RESIZE);
    }

    void TetherWindowManager::SetVisible(const bool visible) const
    {
        m_window->SetVisible(visible);
    }

    void TetherWindowManager::AddResizeCallback(const ResizeCallback& callback)
    {
        m_resizeCallbacks.push_back(callback);
    }

    void TetherWindowManager::SetActiveCamera(const UID cameraID) const
    {
        m_renderTarget->SetActiveCameraID(cameraID);
    }

    bool TetherWindowManager::IsCloseRequested() const
    {
        return m_window->IsCloseRequested();
    }

    Window& TetherWindowManager::GetWindow() const
    {
        return *m_window;
    }

    RenderTarget& TetherWindowManager::GetRenderTarget() const
    {
        return *m_renderTarget;
    }
}

#endif