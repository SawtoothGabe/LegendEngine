#include <LE/Components/Camera.hpp>

namespace le
{
    Camera::Camera()
    {
        CalculateViewMatrix({});
        CalculateProjectionMatrix();
    }

    void Camera::SetAspectRatio(const float aspect)
    {
        m_Aspect = aspect;
        m_CameraDirty = true;
    }

    void Camera::SetFov(const float fov)
    {
        m_Fov = fov;
        m_CameraDirty = true;
    }

    void Camera::SetNearZ(const float nearZ)
    {
        m_NearZ = nearZ;
        m_CameraDirty = true;
    }

    void Camera::SetFarZ(const float farZ)
    {
        m_FarZ = farZ;
        m_CameraDirty = true;
    }

    Vector3f Camera::GetForwardVector()
    {
        return m_ForwardVector;
    }

    Vector3f Camera::GetRightVector()
    {
        return m_RightVector;
    }

    Matrix4x4f Camera::GetCameraMatrix() const
    {
        return m_cameraMatrix;
    }

    bool Camera::IsCameraDirty() const
    {
        return m_CameraDirty;
    }

    void Camera::CalculateViewMatrix(const Transform& transform)
    {
        const Quaternion q = transform.GetRotation();

        m_ForwardVector = Math::Rotate(Math::Inverse(q), Vector3f(0, 0, -1.0f));
        m_RightVector = Math::Rotate(Math::Inverse(q), Vector3f(1.0f, 0, 0));

        auto rot = Matrix4x4f(q);
        const Matrix4x4f pos = Math::Translate(Matrix4x4f::MakeIdentity(), -transform.GetPosition());
        m_view = rot * pos;
        m_cameraMatrix = m_projection * m_view;
    }

    void Camera::CalculateProjectionMatrix()
    {
        m_projection = Math::PerspectiveRH_ZO(Math::Radians(m_Fov),
            m_Aspect, m_NearZ, m_FarZ);
        m_projection[1][1] *= -1;
        m_cameraMatrix = m_projection * m_view;

        m_CameraDirty = false;
    }
}