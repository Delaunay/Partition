#include <Utilities/Math.h>
#include <gtest/gtest.h>

using namespace pn;

namespace MathUnitTest {
TEST(QuaternionTest, SizeTest) { ASSERT_FLOAT_EQ(static_cast<size_t>(16), sizeof(quaternion)); }

TEST(QuaternionTest, InverseTest) {
    quaternion q(0.003f, 0.931f, -0.310f, 0.193f);
    auto       inv = Inverse(q);

    ASSERT_TRUE(q * inv == quaternion::Identity);
}

TEST(QuaternionTest, AxisAngleTest) {
    quaternion q(0.003f, 0.931f, -0.310f, 0.193f);
    q = Normalize(q);

    auto axis_angle = QuaternionToAxisAngle(q);
    ASSERT_TRUE(axis_angle == vec4f(0.003057f, 0.94878f, -0.315920, 2.75317f));
    ASSERT_TRUE(AxisAngleToQuaternion(axis_angle) == q);
}

TEST(QuaternionTest, RotateVectorTest) {
    quaternion q2 = AxisAngleToQuaternion(vec3f(1.0f, 0.0f, 0.0f), Rad(90.0f));
    vec3f      v2(0.0f, 1.0f, 0.0f);
    auto       r2 = RotatePoint(v2, q2);
    ASSERT_TRUE(r2 == vec3f(0.0f, 0.0f, 1.0f));
}

TEST(QuaternionTest, QuaternionToEulerTest) {
    for(float x = -TWOPI; x <= TWOPI; x += 0.1f) {
        for(float y = -TWOPI; y <= TWOPI; y += 0.1f) {
            for(float z = -TWOPI; z <= TWOPI; z += 0.1f) {
                vec3f v(x, y, z);
                auto  q = EulerToQuaternion(v);
                auto  e = QuaternionToEuler(q);

                ASSERT_TRUE(IsRadianEqual(v.x, e.x, 0.01f));
                ASSERT_TRUE(IsRadianEqual(v.y, e.y, 0.01f));
                ASSERT_TRUE(IsRadianEqual(v.z, e.z, 0.01f));
            }
        }
    }
    /*vec3f v(2.0f, -1.32f, 5.82f);
    auto q = EulerToQuaternion(v);
    auto e = QuaternionToEuler(q);
    ASSERT_TRUE(IsRadianEqual(v.x, e.x, 0.0001f));
    ASSERT_TRUE(IsRadianEqual(v.y, e.y, 0.0001f));
    ASSERT_TRUE(IsRadianEqual(v.z, e.z, 0.0001f)); */
}

TEST(QuaternionTest, QuaternionToRotationMatrixTest) {

    {
        const float t = PIDIV2;
        quaternion  q(AxisAngleToQuaternion(vec4f(1.0f, 0.0f, 0.0f, t)));
        q       = Normalize(q);
        mat4f m = QuaternionToRotationMatrix(q);
        ASSERT_TRUE(m == RotationX(t));
    }

    {
        const float t = PIDIV2;
        quaternion  q(AxisAngleToQuaternion(vec4f(0.0f, 1.0f, 0.0f, t)));
        q       = Normalize(q);
        mat4f m = QuaternionToRotationMatrix(q);
        ASSERT_TRUE(m == RotationY(t));
    }

    {
        const float t = PIDIV2;
        quaternion  q(AxisAngleToQuaternion(vec4f(0.0f, 0.0f, 1.0f, t)));
        q       = Normalize(q);
        mat4f m = QuaternionToRotationMatrix(q);
        ASSERT_TRUE(m == RotationZ(t));
    }

    {
        const float t = 0.13f;
        quaternion  q(AxisAngleToQuaternion(vec4f(1.0f, 0.0f, 0.0f, t)));
        q       = Normalize(q);
        mat4f m = QuaternionToRotationMatrix(q);
        ASSERT_TRUE(m == RotationX(t));
    }

    {
        const float t = -1.1f;
        quaternion  q(AxisAngleToQuaternion(vec4f(0.0f, 1.0f, 0.0f, t)));
        q       = Normalize(q);
        mat4f m = QuaternionToRotationMatrix(q);
        ASSERT_TRUE(m == RotationY(t));
    }

    {
        const float t = 3.9f;
        quaternion  q(AxisAngleToQuaternion(vec4f(0.0f, 0.0f, 1.0f, t)));
        q       = Normalize(q);
        mat4f m = QuaternionToRotationMatrix(q);
        ASSERT_TRUE(m == RotationZ(t));
    }
}
} // namespace MathUnitTest