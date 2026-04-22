#pragma once

#include <LE/Graphics/WindowManager.hpp>

namespace le
{
    class DummyWindowManager : public WindowManager
    {
    public:
        void SetVisible(bool visible) const override;
        void AddResizeCallback(const ResizeCallback& callback) override;
        void SetActiveCamera(UID cameraID) const override;
        bool IsCloseRequested() const override;
#ifndef LE_HEADLESS
        [[nodiscard]] Tether::Window& GetWindow() override;
#endif
        [[nodiscard]] RenderTarget& GetRenderTarget() const override;
    };
}
