#include <LE/Graphics/WindowManager.hpp>

#ifndef LE_HEADLESS
#include <LE/Graphics/TetherWindowManager.hpp>
#endif
#include <LE/Graphics/Dummy/DummyWindowManager.hpp>

namespace le
{
    void WindowManager::PollEvents()
    {
#ifndef LE_HEADLESS
        Tether::Application::Get().PollEvents();
#endif
    }

    Scope<WindowManager> WindowManager::Create(GraphicsResources& resources, std::string_view title, int width,
        int height)
    {
#ifdef LE_HEADLESS
        return std::make_unique<DummyWindowManager>();
#else
        return std::make_unique<TetherWindowManager>(resources, title, width, height);
#endif
    }
}
