#pragma once

namespace le
{
    class RenderTarget
    {
    public:
        virtual ~RenderTarget() = default;

        virtual void SetVSync(bool vsync) = 0;
        virtual void InvalidateSwapchain() = 0;
        void SetActiveCameraID(UID cameraID);
        UID GetActiveCameraID() const;
    private:
        UID m_cameraID = 0;
    };
}