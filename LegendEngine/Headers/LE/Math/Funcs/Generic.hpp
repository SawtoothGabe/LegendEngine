#pragma once

#include <LE/Math/Constants.hpp>

namespace le::Math
{
	template<typename T>
	T Abs(T arg0)
	{
		return arg0 > 0 ? arg0 : -arg0;
	}

	template<typename T>
	T Max(T arg0, T arg1)
	{
		return arg0 < arg1 ? arg1 : arg0;
	}

	template<typename T>
	T Radians(T degrees)
	{
		return degrees * PI / 180;
	}

	template<typename T>
	T Degrees(T radians)
	{
		return radians * 180 / PI;
	}
}
