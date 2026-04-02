#pragma once

#include <LE/Components/Component.hpp>
#include <LE/Math/Math.hpp>
#include <LE/Math/Quaternion.hpp>

namespace le
{
    struct Transform : Component
    {
        void SetPosition(const Vector3f& position);
        void SetScale(const Vector3f& scale);
        void SetRotation(const Vector3f& rotation);
        void SetRotation(const Quaternion& q);

        [[nodiscard]] Vector3f GetPosition() const;
        Vector3f GetScale();
        [[nodiscard]] Quaternion GetRotation() const;

        void AddPosition(const Vector3f& position);
        void AddScale(const Vector3f& scale);

        void CalculateTransformMatrix();

        bool dirty = false;
        Matrix4x4f transformMat;
    private:
        Vector3f m_position;
        Vector3f m_scale = Vector3f(1.0f);
        Quaternion m_rotation;
    };
}
