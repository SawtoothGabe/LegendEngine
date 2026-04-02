#pragma once

#include <cmath>

#include <LE/Math/Quaternion.hpp>

namespace le::Math
{
	Quaternion Normalize(const Quaternion& q);
	float Dot(const Quaternion& q1, const Quaternion& q2);
	float Length(const Quaternion& q);
	Quaternion Conjugate(const Quaternion& q);
	Quaternion Inverse(const Quaternion& q);

	template<typename T>
	Quaternion AngleAxis(T angle, Vector<T, 3> axis)
	{
		T halfAngle = angle / T(2);
		Vector<T, 3> saxis = axis * std::sin(halfAngle);

		return Quaternion(std::cos(halfAngle), saxis.x, saxis.y, saxis.z);
	}

	template<typename T>
	Quaternion Euler(Vector<T, 3> vec)
	{
		Vector<T, 3> halfVec(vec);
		halfVec /= T(2);

		const float cx = std::cos(halfVec.x);
		const float cy = std::cos(halfVec.y);
		const float cz = std::cos(halfVec.z);

		const float sx = std::sin(halfVec.x);
		const float sy = std::sin(halfVec.y);
		const float sz = std::sin(halfVec.z);

		Quaternion q;
		q.w = cx * cy * cz + sx * sy * sz;
		q.x = sx * cy * cz - cx * sy * sz;
		q.y = cx * sy * cz + sx * cy * sz;
		q.z = cx * cy * sz - sx * sy * cz;

		return q;
	}

	template<typename T>
	Vector<T, 3> Rotate(Quaternion q, Vector<T, 3> axis)
	{
		return q * axis;
	}
}
