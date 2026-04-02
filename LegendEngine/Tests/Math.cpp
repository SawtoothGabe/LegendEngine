#include <gtest/gtest.h>

#include <print>

#include <LE/Math/Math.hpp>
#include <LE/Math/Vector.hpp>

using namespace le;
using namespace Math;

TEST(Math, VectorConstruction)
{
    const Vector<int, 2> singleValue(1);
    const Vector<int, 3> multipleValues(1, 2, 3);

    EXPECT_EQ(singleValue.x, 1);
    EXPECT_EQ(singleValue.y, 1);

    EXPECT_EQ(multipleValues.x, 1);
    EXPECT_EQ(multipleValues.y, 2);
    EXPECT_EQ(multipleValues.z, 3);

    const Vector<int, 4> defaultValue;
    EXPECT_EQ(defaultValue.x, 0);
    EXPECT_EQ(defaultValue.y, 0);
    EXPECT_EQ(defaultValue.z, 0);
    EXPECT_EQ(defaultValue.w, 0);
}

TEST(Math, VectorFromVectors)
{
    const Vector<int, 3> a(1, 2, 3);
    const Vector<int, 4> b(a, 4);

    EXPECT_EQ(b.x, 1);
    EXPECT_EQ(b.y, 2);
    EXPECT_EQ(b.z, 3);
    EXPECT_EQ(b.w, 4);

    const Vector<int, 2> c(1, 2);
    const Vector<int, 4> d(c, 3, 4);

    EXPECT_EQ(d.x, 1);
    EXPECT_EQ(d.y, 2);
    EXPECT_EQ(d.z, 3);
    EXPECT_EQ(d.w, 4);
}

TEST(Math, VectorCopying)
{
    const Vector<int, 3> a(1, 2, 3);
    const Vector<int, 3> b = a;

    EXPECT_EQ(b.x, 1);
    EXPECT_EQ(b.y, 2);
    EXPECT_EQ(b.z, 3);
}

TEST(Math, VectorAddition)
{
    Vector<int, 3> a(1, 2, 3);
    const Vector<int, 3> b(1, 2, 3);

    a += b;
    a += 3;

    Vector<int, 3> c = a + b;

    EXPECT_EQ(c.x, 6);
    EXPECT_EQ(c.y, 9);
    EXPECT_EQ(c.z, 12);
}

TEST(Math, VectorSubtraction)
{
    Vector<int, 3> a(1, 2, 3);
    const Vector<int, 3> b(1, 2, 3);

    a -= b;
    a -= 3;

    Vector<int, 3> c = a - b;

    EXPECT_EQ(c.x, -4);
    EXPECT_EQ(c.y, -5);
    EXPECT_EQ(c.z, -6);
}

TEST(Math, VectorMultiplication)
{
    Vector<int, 3> a(1, 2, 3);
    const Vector<int, 3> b(1, 2, 3);

    a *= b;
    a *= 3;

    Vector<int, 3> c = a * b;

    EXPECT_EQ(c.x, 3);
    EXPECT_EQ(c.y, 24);
    EXPECT_EQ(c.z, 81);
}

TEST(Math, VectorDivision)
{
    Vector<int, 3> a(10, 20, 30);
    const Vector<int, 3> b(1, 2, 3);

    a /= b;
    a /= 3;

    Vector<int, 3> c = a / b;

    EXPECT_EQ(c.x, 3);
    EXPECT_EQ(c.y, 1);
    EXPECT_EQ(c.z, 1);
}

TEST(Math, VectorLength)
{
    const Vector<int, 3> a(10, 0, 0);
    const Vector<int, 3> b(0, 10, 0);
    const Vector<int, 3> c(0, 0, 10);

    EXPECT_EQ(a.Length(), 10);
    EXPECT_EQ(b.Length(), 10);
    EXPECT_EQ(c.Length(), 10);
}

TEST(Math, VectorNormalize)
{
    Vector<int, 3> a(1, 2, 3);
    a.Normalize();

    EXPECT_EQ(a.x, 0);
    EXPECT_EQ(a.y, 0);
    EXPECT_EQ(a.z, 1);

    Vector<float, 3> b(1, 1, 0);
    b.Normalize();

    EXPECT_EQ(b.y, std::sqrt(2.0f) / 2.0f);
    EXPECT_EQ(b.x, std::sqrt(2.0f) / 2.0f);
    EXPECT_EQ(b.z, 0);
}

TEST(Math, NormalizeZeroLength)
{
    Vector<int, 3> a(0, 0, 0);
    a.Normalize();

    EXPECT_EQ(a.x, 0);
    EXPECT_EQ(a.y, 0);
    EXPECT_EQ(a.z, 0);
}

TEST(Math, DotProduct)
{
    Vector<int, 3> a(1, 2, 3);
    const Vector<int, 3> b(1, 2, 3);
    const int c = Dot(a ,b);

    EXPECT_EQ(c, 14);
}
