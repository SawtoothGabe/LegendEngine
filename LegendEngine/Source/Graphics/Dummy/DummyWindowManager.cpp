#include <LE/Graphics/Dummy/DummyWindowManager.hpp>

namespace le
{
    void DummyWindowManager::SetVisible(bool visible) const {}
    void DummyWindowManager::AddResizeCallback(const ResizeCallback& callback) {}
    void DummyWindowManager::SetActiveCamera(UID cameraID) const {}

    bool DummyWindowManager::IsCloseRequested() const
    {
        return true;
    }

#ifndef LE_HEADLESS
    Tether::Window& DummyWindowManager::GetWindow() const
    {
        throw std::runtime_error("DummyWindowManager::GetWindow() is not implemented");
    }
#endif

    RenderTarget& DummyWindowManager::GetRenderTarget() const
    {
        throw std::runtime_error("DummyWindowManager::GetRenderTarget() is not implemented");
    }
}
