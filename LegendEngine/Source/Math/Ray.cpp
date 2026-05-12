#include <LE/Math/Ray.hpp>

namespace le
{
    Ray::Ray(const Vector3f& origin, const Vector3f& direction)
    {
        org = origin;
        dir = direction;
        CalculateInverse();
    }

    void Ray::CalculateInverse()
    {
        inv = dir.Map([](const float x)
        {
            if (x == 0.0f) // We don't want negative infinity when x is -0.0f
                return std::numeric_limits<float>::infinity();

            return 1.0f / x;
        });
    }
}
