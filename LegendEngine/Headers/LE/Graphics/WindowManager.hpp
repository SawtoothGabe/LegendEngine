#pragma once

#include <LE/Common/UID.hpp>
#include <LE/Graphics/GraphicsResources.hpp>


namespace le
{
    class WindowManager
    {
    public:
        using ResizeCallback = std::function<void(int, int)>;

        WindowManager() = default;
        virtual ~WindowManager() = default;
        LE_NO_COPY(WindowManager);

        virtual void SetVisible(bool visible) const = 0;
        virtual void AddResizeCallback(const ResizeCallback& callback) = 0;
        virtual void SetActiveCamera(UID cameraID) const = 0;

        [[nodiscard]] virtual bool IsCloseRequested() const = 0;

#ifndef LE_HEADLESS
        [[nodiscard]] virtual Tether::Window& GetWindow() = 0;
#endif
        [[nodiscard]] virtual RenderTarget& GetRenderTarget() const = 0;

        static void PollEvents();
        static Scope<WindowManager> Create(GraphicsResources& resources, std::string_view title, int width, int height);
    };
}
