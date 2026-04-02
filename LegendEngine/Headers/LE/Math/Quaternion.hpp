#pragma once

#include <cmath>

#include <LE/Math/Constants.hpp>
#include <LE/Math/Vector.hpp>
#include <LE/Math/Funcs/VectorFuncs.hpp>

namespace le
{
	class Quaternion
	{
	public:
		Quaternion() = default;
		Quaternion(const float w, const float x, const float y, const float z)
			:
			w(w),
			x(x),
			y(y),
			z(z)
		{}
		
		Quaternion(const Quaternion& ref)
		{
			w = ref.w;
			x = ref.x;
			y = ref.y;
			z = ref.z;
		}

		Quaternion& operator=(const Quaternion& ref) = default;

		template<typename T>
		Vector<T, 3> operator*(const Vector<T, 3>& v)
		{
			Vector<T, 3> quatVec(x, y, z);
			Vector<T, 3> uv(Math::Cross(quatVec, v));
			Vector<T, 3> uuv(Math::Cross(quatVec, uv));

			return v + (uv * w + uuv) * static_cast<T>(2);
		}

		Quaternion operator/(const float v) const
		{
			return { w / v, x / v, y / v, z / v };
		}

		Quaternion operator*(const Quaternion& q)
		{
			return Quaternion(*this) *= q;
		}

		Quaternion& operator*=(const Quaternion& q)
		{
			Quaternion p(*this);

			w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
			x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
			y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
			z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
			return *this;
		}
		
		/**
		 * @brief Converts the quaternion to Euler angles.
		 * 
		 * @returns The converted Euler angles.
		 */
		[[nodiscard]] Vector<float, 3> GetEulerAngles() const
		{
			Vector<float, 3> angles;

			// X
			float sinr_cosp = 2.0f * (w * x + y * z);
			float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
			angles.x = std::atan2(sinr_cosp, cosr_cosp);

			// Y
			float sinp = 2.0f * (w * y - z * x);
			if (std::abs(sinp) >= 1)
				angles.y = std::copysign(Math::PI / 2.0f, sinp);
			else
				angles.y = std::asin(sinp);

			// Z
			float siny_cosp = 2.0f * (w * z + x * y);
			float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
			angles.z = std::atan2(siny_cosp, cosy_cosp);

			return angles;
		}

		float w = 1.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
}
