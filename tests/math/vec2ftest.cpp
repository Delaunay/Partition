#include <Utilities/Math.h>
#include <gtest/gtest.h>

using namespace pn;

namespace MathUnitTest {
TEST(Vec2fTest, SizeTest) { ASSERT_FLOAT_EQ(static_cast<size_t>(8), sizeof(vec2f)); }

TEST(Vec2fTest, ConstructorTest) {
    {
        vec2f v;
        ASSERT_FLOAT_EQ(0.0f, v.x);
        ASSERT_FLOAT_EQ(0.0f, v.y);
    }

    {
        vec2f v{};
        ASSERT_FLOAT_EQ(0.0f, v.x);
        ASSERT_FLOAT_EQ(0.0f, v.y);
    }

    {
        vec2f v(1.0f, -3.0f);
        ASSERT_FLOAT_EQ(1.0f, v.x);
        ASSERT_FLOAT_EQ(-3.0f, v.y);

        {
            vec2f v2(v);
            ASSERT_FLOAT_EQ(1.0f, v2.x);
            ASSERT_FLOAT_EQ(-3.0f, v2.y);
        }

        {
            vec2f v2 = v;
            ASSERT_FLOAT_EQ(1.0f, v2.x);
            ASSERT_FLOAT_EQ(-3.0f, v2.y);
        }
    }

    {
        vec2f v(2.0, 4.0);
        ASSERT_FLOAT_EQ(2.0f, v.x);
        ASSERT_FLOAT_EQ(4.0f, v.y);
    }

    {
        vec2f v(2, 4);
        ASSERT_FLOAT_EQ(2.0f, v.x);
        ASSERT_FLOAT_EQ(4.0f, v.y);
    }
}

TEST(Vec2fTest, VectorAdditionTest) {
    vec2f v1(3.0f, 1.0f);
    vec2f v2(-7.0f, 2.0f);
    vec2f sum = v1 + v2;
    ASSERT_FLOAT_EQ(-4.0f, sum.x);
    ASSERT_FLOAT_EQ(3.0f, sum.y);
}

TEST(Vec2fTest, VectorSubtractionTest) {
    vec2f v1(3.0f, 1.0f);
    vec2f v2(-7.0f, 2.0f);

    {
        vec2f diff = v1 - v2;
        ASSERT_FLOAT_EQ(10.0f, diff.x);
        ASSERT_FLOAT_EQ(-1.0f, diff.y);
    }

    {
        vec2f diff = v2 - v1;
        ASSERT_FLOAT_EQ(-10.0f, diff.x);
        ASSERT_FLOAT_EQ(1.0f, diff.y);
    }
}

TEST(Vec2fTest, VectorMultTest) {
    vec2f u(2.0f, 10.0f);
    vec2f v(10.0f, 40.0f);
    ASSERT_TRUE(u * v == vec2f(20.0f, 400.0f));

    ASSERT_TRUE(u * vec2f::Zero == vec2f::Zero);
    ASSERT_TRUE(u * vec2f::One == u);
}

TEST(Vec2fTest, VectorDivTest) {
    vec2f u(10.0f, 40.0f);
    vec2f v(2.0f, 10.0f);
    ASSERT_TRUE(u / v == vec2f(5.0f, 4.0f));

    ASSERT_TRUE(u / vec2f::One == u);
}

TEST(Vec2fTest, ScalarMultTest) {
    vec2f v(6.0f, 10.0f);
    float c = 3.0f;

    {
        vec2f p = v * c;
        ASSERT_FLOAT_EQ(18.0f, p.x);
        ASSERT_FLOAT_EQ(30.0f, p.y);
    }

    {
        vec2f p = c * v;
        ASSERT_FLOAT_EQ(18.0f, p.x);
        ASSERT_FLOAT_EQ(30.0f, p.y);
    }
}

TEST(Vec2fTest, ScalarDivTest) {
    vec2f v(6.0f, 30.0f);
    float c = 3.0f;

    {
        vec2f p = v / c;
        ASSERT_FLOAT_EQ(2.0f, p.x);
        ASSERT_FLOAT_EQ(10.0f, p.y);
    }
}

TEST(Vec2fTest, EqualOperatorTest) {
    vec2f v1(1.0f, 340.0f);

    {
        vec2f v2(v1);
        ASSERT_TRUE(v1 == v2);
        ASSERT_TRUE(v2 == v1);
        ASSERT_FALSE(v1 != v2);
        ASSERT_FALSE(v2 != v1);
    }
}

TEST(Vec2fTest, AddOpTest) {
    vec2f v(4.0f, 10.0f);
    v += vec2f(10.0f, -10.0f);
    ASSERT_TRUE(v == vec2f(14.0f, 0.0f));
}

TEST(Vec2fTest, SubOpTest) {
    vec2f v(4.0f, 10.0f);
    v -= vec2f(10.0f, -10.0f);
    ASSERT_TRUE(v == vec2f(-6.0f, 20.0f));
}

TEST(Vec2fTest, MultOpTest) {
    vec2f v(4.0f, 10.0f);
    v *= 2.0f;
    ASSERT_TRUE(v == vec2f(8.0f, 20.0f));
}

TEST(Vec2fTest, DivOpTest) {
    vec2f v(4.0f, 10.0f);
    v /= 2.0f;
    ASSERT_TRUE(v == vec2f(2.0f, 5.0f));
}

TEST(Vec2fTest, DivVectorOpTest) {
    vec2f v(4.0f, 10.0f);
    vec2f v2(2.0f, 5.0f);
    v /= v2;
    ASSERT_TRUE(v == vec2f(2.0f, 2.0f));
}

TEST(Vec2fTest, MultVectorOpTest) {
    vec2f v(4.0f, 10.0f);
    vec2f v2(2.0f, 5.0f);
    v *= v2;
    ASSERT_TRUE(v == vec2f(8.0f, 50.0f));
}

TEST(Vec2fTest, LengthTest) {
    {
        vec2f v(3.0f, 4.0f);
        ASSERT_FLOAT_EQ(25.0f, LengthSqr(v));
        ASSERT_FLOAT_EQ(5.0f, Length(v));
    }

    {
        vec2f v;
        ASSERT_FLOAT_EQ(0.0f, LengthSqr(v));
        ASSERT_FLOAT_EQ(0.0f, Length(v));
    }
}

TEST(Vec2fTest, NormalizeTest) {
    vec2f v(4.0f, 10.0f);
    ASSERT_NE(1.0f, Length(v));

    vec2f n = Normalize(v);
    ASSERT_FLOAT_EQ(1.0f, Length(n));
}

TEST(Vec2fTest, ClassConstantsTest) {
    ASSERT_TRUE(vec2f::Zero == vec2f(0.0f, 0.0f));
    ASSERT_TRUE(vec2f::One == vec2f(1.0f, 1.0f));
    ASSERT_TRUE(vec2f::UnitX == vec2f(1.0f, 0.0f));
    ASSERT_TRUE(vec2f::UnitY == vec2f(0.0f, 1.0f));
}

TEST(Vec2fTest, DistanceTest) {
    vec2f v1(7.0f, 3.0f);
    vec2f v2(10.0f, 7.0f);
    ASSERT_FLOAT_EQ(25.0f, DistanceSqr(v1, v2));
    ASSERT_FLOAT_EQ(5.0f, Distance(v1, v2));
}

TEST(Vec2fTest, DotTest) {
    vec2f v1(3.0f, 2.0f);
    vec2f v2(2.0f, 2.0f);
    ASSERT_FLOAT_EQ(10.0f, Dot(v1, v2));
}

TEST(Vec2fTest, LerpTest) {
    vec2f u(3.0f, 1.0f);
    vec2f v(5.0f, 7.0f);
    ASSERT_TRUE(Lerp(u, v, 0.0f) == u);
    ASSERT_TRUE(Lerp(u, v, 1.0f) == v);
    ASSERT_TRUE(Lerp(u, v, 0.5f) == vec2f(4.0f, 4.0f));
}

TEST(Vec2fTest, ClampTest) {
    vec2f v(3.0f, 10.0f);
    vec2f min(4.0f, 7.0f);
    vec2f max(10.0f, 9.0f);
    ASSERT_TRUE(Clamp(v, min, max) == vec2f(4.0f, 9.0f));
}

TEST(Vec2fTest, MinTest) {
    vec2f v(10.0f, -10.0f);
    vec2f v2(12.0f, -7.0f);
    ASSERT_TRUE(Min(v, v2) == vec2f(10.0f, -10.0f));
}

TEST(Vec2fTest, MaxTest) {
    vec2f v(10.0f, -10.0f);
    vec2f v2(12.0f, -7.0f);
    ASSERT_TRUE(Max(v, v2) == vec2f(12.0f, -7.0f));
}

TEST(Vec2fTest, SmoothStepTest) {
    vec2f edge0(2.0f, 2.0f);
    vec2f edge1(5.0f, 5.0f);
    vec2f t(3.0f, 3.0f);
    vec2f result = SmoothStep(edge0, edge1, t);
    ASSERT_TRUE(vec2f::One * (7.0f / 27.0f) == result);
}

TEST(Vec2fTest, AngleBetweenTest) {
    {
        vec2f u(1.0f, 0.0f);
        vec2f v(0.0f, 1.0f);
        ASSERT_TRUE(IsRadianEqual(PIDIV2, AngleBetween(u, v)));
        ASSERT_TRUE(IsRadianEqual(PIDIV2, AngleBetween(v, u)));
    }

    {
        vec2f u(-1.0f, 0.0f);
        vec2f v(0.0f, 1.0f);
        ASSERT_TRUE(IsRadianEqual(PIDIV2, AngleBetween(u, v)));
        ASSERT_TRUE(IsRadianEqual(PIDIV2, AngleBetween(v, u)));
    }
}
} // namespace MathUnitTest