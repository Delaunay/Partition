#include <Utilities/Math.h>
#include <gtest/gtest.h>

using namespace pn;

static const float EPS = 0.00001f;

namespace MathUnitTest {
TEST(FloatTest, SmoothStepTest) {
    float edge0 = 4.0f;
    float edge1 = 11.0f;

    {
        float x = 1.0f;
        ASSERT_FLOAT_EQ(0.0f, SmoothStep(edge0, edge1, x));
    }

    {
        float x = 40.0f;
        ASSERT_FLOAT_EQ(1.0f, SmoothStep(edge0, edge1, x));
    }

    {
        float x = 6.0f;
        ASSERT_FLOAT_EQ(0.19825099f, SmoothStep(edge0, edge1, x), EPS);
    }
}

TEST(FloatTest, ClampTest) {
    float min = 1.0f;
    float max = 40.0f;

    {
        float x = -10.0f;
        ASSERT_FLOAT_EQ(min, Clamp(x, min, max));
    }

    {
        float x = 30.0f;
        ASSERT_FLOAT_EQ(x, Clamp(x, min, max));
    }

    {
        float x = 100.0f;
        ASSERT_FLOAT_EQ(max, Clamp(x, min, max));
    }
}

TEST(FloatTest, LerpTest) {
    float x1 = 0.0f;
    float x2 = 10.0f;

    ASSERT_DOUBLE_EQ(x1, Lerp(x1, x2, 0.0f));
    ASSERT_DOUBLE_EQ(x2, Lerp(x1, x2, 1.0f));
    ASSERT_DOUBLE_EQ(5.0f, Lerp(x1, x2, 0.5f));
}

TEST(FloatTest, AngleEqualTest) {

    ASSERT_TRUE(IsAngleEqual(0.0f, 0.0f));
    ASSERT_TRUE(IsAngleEqual(0.0f, 360.0f));
    ASSERT_TRUE(IsAngleEqual(0.0f, -360.0f));

    ASSERT_TRUE(IsAngleEqual(180.0f, -180.0f));
    ASSERT_TRUE(IsAngleEqual(180.0f, 180.0f + 360.0f));
    ASSERT_TRUE(IsAngleEqual(180.0f, 180.0f - 360.0f));

    ASSERT_TRUE(IsAngleEqual(10.0f, 10.0f));
    ASSERT_TRUE(IsAngleEqual(-10.0f, 350.0f));
    ASSERT_TRUE(IsAngleEqual(20.0f, 380.0f));

    ASSERT_FALSE(IsAngleEqual(0.0f, 1.0f));
    ASSERT_FALSE(IsAngleEqual(0.0f, -1.0f));
}

TEST(FloatTest, RadianEqualTest) {
    ASSERT_TRUE(IsRadianEqual(0.0f, 0.0f));
    ASSERT_TRUE(IsRadianEqual(0.0f, TWOPI));
    ASSERT_TRUE(IsRadianEqual(0.0f, -TWOPI));

    ASSERT_TRUE(IsRadianEqual(Rad(180.0f), Rad(-180.0f)));

    ASSERT_FALSE(IsRadianEqual(Rad(0.0f), Rad(1.0f)));
    ASSERT_FALSE(IsRadianEqual(Rad(0.0f), Rad(-1.0f)));

    ASSERT_TRUE(IsRadianEqual(PIDIV2, PIDIV2));
    ASSERT_TRUE(IsRadianEqual(PIDIV2, PIDIV2 + TWOPI));
    ASSERT_TRUE(IsRadianEqual(PIDIV2, PIDIV2 - TWOPI));

    ASSERT_TRUE(IsRadianEqual(Rad(10.0f), Rad(10.0f)));
    ASSERT_TRUE(IsRadianEqual(Rad(-10.0f), Rad(350.0f)));
    ASSERT_TRUE(IsRadianEqual(Rad(20.0f), Rad(380.0f)));
}
} // namespace MathUnitTest