#pragma once

#include <LE/Math/Matrix4x4.hpp>
#include <LE/Math/Vector.hpp>
#include <Tether/Math/Types.hpp>

namespace le
{
	// Matrix types
	using Matrix4x4f = Matrix4x4<float>;

	// Vector types
	using Color3 = Vector<float, 3>;
	using Color = Vector<float, 4>;

	template <typename T>
	using Vector2 = Vector<T, 2>;

	template <typename T>
	using Vector3 = Vector<T, 3>;

	template <typename T>
	using Vector4 = Vector<T, 4>;

	using Vector2f = Vector2<float>;
	using Vector3f = Vector3<float>;
	using Vector4f = Vector4<float>;
}
