#pragma once

#include <LE/Components/Component.hpp>
#include <LE/Components/Transform.hpp>
#include <LE/Math/Math.hpp>

namespace le
{
    class Camera final : public Component
    {
    public:
        struct CameraUniforms
        {
            Matrix4x4f cameraMatrix;
        };

        Camera();

        void SetAspectRatio(float aspect);

        void SetFov(float fov);
        void SetNearZ(float nearZ);
        void SetFarZ(float farZ);

        Vector3f GetForwardVector();
        Vector3f GetRightVector();

        [[nodiscard]] Matrix4x4f GetCameraMatrix() const;

        [[nodiscard]] bool IsCameraDirty() const;

        void CalculateViewMatrix(const Transform& transform);
        void CalculateProjectionMatrix();

        const Vector3f UP = Vector3f(0, 1, 0);
    private:
        float m_Fov = 90.0f;
        float m_Aspect = 16.0f / 9.0f;
        float m_NearZ = 0.1f;
        float m_FarZ = 1000.0f;

        Vector3f m_ForwardVector;
        Vector3f m_RightVector;

        Matrix4x4f m_view;
        Matrix4x4f m_projection;
        Matrix4x4f m_cameraMatrix;

        bool m_CameraDirty = false;
    };
}