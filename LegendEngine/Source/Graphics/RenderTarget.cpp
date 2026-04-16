#include <LE/Graphics/RenderTarget.hpp>

namespace le
{
    void RenderTarget::SetActiveCameraID(const UID cameraID)
    {
        m_cameraID = cameraID;
    }

    UID RenderTarget::GetActiveCameraID() const
    {
        return m_cameraID;
    }
}
