#pragma once

#include <cmath>

#include <LE/Math/Matrix4x4.hpp>

namespace le::Math
{
	template<typename T>
	Matrix4x4<T> Scale(Matrix4x4<T> m, Vector<T, 3> scale)
	{
		Matrix4x4<T> product;
		product.c[0] = m.c[0] * scale.x;
		product.c[1] = m.c[1] * scale.y;
		product.c[2] = m.c[2] * scale.z;
		product.c[3] = m.c[3];

		return product;
	}

	/**
	 * @param rotation The rotation in degrees.
	 */
	template<typename T>
	Matrix4x4<T> Rotate(Matrix4x4<T> m, T a, Vector<T, 3> rotation)
	{
		T ca = cos(a);
		T sa = sin(a);

		Vector<T, 3> axis = Vector<T, 3>::Normalize(rotation);
		Vector<T, 3> temp(axis * (T(1) - ca));

		Matrix4x4<T> product;
		product[0][0] = ca + temp[0] * axis[0];
		product[0][1] = temp[0] * axis[1] + sa * axis[2];
		product[0][2] = temp[0] * axis[2] - sa * axis[1];

		product[1][0] = temp[1] * axis[0] - sa * axis[2];
		product[1][1] = ca + temp[1] * axis[1];
		product[1][2] = temp[1] * axis[2] + sa * axis[0];

		product[2][0] = temp[2] * axis[0] + sa * axis[1];
		product[2][1] = temp[2] * axis[1] - sa * axis[0];
		product[2][2] = ca + temp[2] * axis[2];

		Matrix4x4<T> result;
		result.c[0] = m.c[0] * product.c[0].x + m.c[1] * product.c[0].y + m.c[2] * product.c[0].z;
		result.c[1] = m.c[0] * product.c[1].x + m.c[1] * product.c[1].y + m.c[2] * product.c[1].z;
		result.c[2] = m.c[0] * product.c[2].x + m.c[1] * product.c[2].y + m.c[2] * product.c[2].z;
		result.c[3] = m.c[3];

		return result;
	}

	template<typename T>
	Matrix4x4<T> Translate(Matrix4x4<T> m, Vector<T, 3> pos)
	{
		Matrix4x4<T> product = m;
		product.c[3] = m[0] * pos.x + m[1] * pos.y + m[2] * pos.z + m[3];

		return product;
	}

	/**
	 * @brief Constructs a view matrix based on where the camera is looking.
	 */
	template<typename T>
	Matrix4x4<T> LookAt(Vector<T, 3> position, Vector<T, 3> target,
		Vector<T, 3> up)
	{
		Vector<T, 3> centerDir = Vector<T, 3>::Normalize(target - position);
		Vector<T, 3> centerUp = Vector<T, 3>::Normalize(Vector<T, 3>::Cross(centerDir, up));
		Vector<T, 3> both = Vector<T, 3>::Cross(centerUp, centerDir);

		Matrix4x4<T> product = Matrix4x4<T>::MakeIdentity();
		product[0][0] = centerUp.x;
		product[1][0] = centerUp.y;
		product[2][0] = centerUp.z;
		product[0][1] = both.x;
		product[1][1] = both.y;
		product[2][1] = both.z;
		product[0][2] = -centerDir.x;
		product[1][2] = -centerDir.y;
		product[2][2] = -centerDir.z;
		product[3][0] = -Vector<T, 3>::Dot(centerUp, position);
		product[3][1] = -Vector<T, 3>::Dot(both, position);
		product[3][2] =  Vector<T, 3>::Dot(centerDir, position);

		return product;
	}

	/**
	 * @brief Creates an orthographic projection matrix.
	 *
	 * @param left The left coordinate of the frustum.
	 * @param right The right coordinate of the frustum.
	 * @param bottom The bottom coordinate of the frustum.
	 * @param top The top coordinate of the frustum.
	 * @param near Distance between the near plane.
	 * @param far Distance between the far plane.
	 */
	template<typename T>
	Matrix4x4<T> Ortho(T left, T right, T bottom, T top, T near, T far)
	{
		Matrix4x4<T> product = Matrix4x4<T>::MakeIdentity();
		product[0][0] =  T(2) / (right - left);
		product[1][1] =  T(2) / (top - bottom);
		product[2][2] = -T(2) / (near - far);
		product[3][0] = -(right + left) / (right - left);
		product[3][1] = -(top + bottom) / (top - bottom);
		product[3][2] = -(far + near)   / (far - near);

		return product;
	}

	/**
	 * @brief Creates a perspective projection matrix.
	 *	DirectX coordinates (0 and 1)
	 *
	 * @param fov The vertical field of view in radians.
	 * @param aspect The aspect ratio of the window. Width / Height.
	 * @param nearZ Distance between the near plane.
	 * @param farZ Distance between the far plane.
	 */
	template<typename T>
	Matrix4x4<T> PerspectiveRH_ZO(T fov, T aspect, T nearZ, T farZ)
	{
		const T tanHalfFov = tan(fov / static_cast<T>(2));

		Matrix4x4<T> product;
		product[0][0] = T(1) / (aspect * tanHalfFov);
		product[1][1] = T(1) / (tanHalfFov);
		product[2][2] = farZ / (nearZ - farZ);
		product[2][3] = -T(1);
		product[3][2] = -(farZ * nearZ) / (farZ - nearZ);

		return product;
	}

	/**
	 * @brief Creates a perspective projection matrix.
	 *  OpenGL coordinates (-1 and 1)
	 *
	 * @param fov The vertical field of view in radians.
	 * @param aspect The aspect ratio of the window. Width / Height.
	 * @param nearZ Distance between the near plane.
	 * @param farZ Distance between the far plane.
	 */
	template<typename T>
	Matrix4x4<T> PerspectiveRH_NO(T fov, T aspect, T nearZ, T farZ)
	{
		const T tanHalfFov = tan(fov / static_cast<T>(2));

		Matrix4x4<T> product;
		product[0][0] = T(1) / (aspect * tanHalfFov);
		product[1][1] = T(1) / (tanHalfFov);
		product[2][2] = -(farZ + nearZ) / (farZ - nearZ);
		product[2][3] = -T(1);
		product[3][2] = -(T(2) * farZ * nearZ) / (farZ - nearZ);

		return product;
	}
}
