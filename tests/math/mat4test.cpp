#include <gtest/gtest.h>

#include <DirectXMath.h>
#include <Utilities/Math.h>

#include <string>

using namespace pn;

namespace MathUnitTest {
TEST(Mat4fTest, SizeTest) { ASSERT_FLOAT_EQ(static_cast<size_t>(64), sizeof(mat4f)); }

TEST(Mat4fTest, ConstructorTest) {
    mat4f m(1.0f, 2.0f, 3.0f, -4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, -11.0f, 12.0f, 13.0f,
            -14.0f, 15.0f, 16.0f);

    mat4f m2(m);

    ASSERT_TRUE(m == m2);

    mat4f m3 = m;
    ASSERT_TRUE(m == m3);

    mat4f m4;
    ASSERT_TRUE(m4 == mat4f::Identity);
}

TEST(Mat4fTest, AssignmentOp) {
    mat4f m(1.0f, 2.0f, 3.0f, -4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, -11.0f, 12.0f, 13.0f,
            -14.0f, 15.0f, 16.0f);

    mat4f m2;
    m2 = m;
    ASSERT_TRUE(m2 == m);
}

TEST(Mat4fTest, MatrixMultTest) {
    mat4f m(1.0f, 2.0f, 3.0f, -4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, -11.0f, 12.0f, 13.0f,
            -14.0f, 15.0f, 16.0f);

    mat4f m2(2.0f, 1.0f, 3.0f, 1.0f, 5.0f, -2.0f, 6.0f, 10.0f, -12.0f, 13.0f, 4.0f, 5, 1, -1, 2, 9);

    mat4f result(-28, 40, 19, 0, -36, 76, 95, 172, 212, -166, 67, 162, -208, 220, 47, 92);

    ASSERT_TRUE(m * mat4f::Identity == m);
    ASSERT_TRUE(m * mat4f::Zero == mat4f::Zero);
    ASSERT_TRUE((m * m2) == result);
}

TEST(Mat4fTest, InverseTest) {
    ASSERT_TRUE(Inverse(mat4f::Identity) == mat4f::Identity);

    mat4f m(1.0f, 2.0f, 3.0f, -4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, -11.0f, 12.0f, 13.0f,
            -14.0f, 15.0f, 16.0f);

    auto i = Inverse(m);
    ASSERT_TRUE(m * i == mat4f::Identity);
    ASSERT_TRUE(i * m == mat4f::Identity);
    auto j = Inverse(i);
    ASSERT_TRUE(j == m);
}

TEST(Mat4fTest, TransposeTest) {
    mat4f m(
		1.0f, 2.0f, 3.0f, -4.0f, 
		5.0f, 6.0f, 7.0f, 8.0f, 
		9.0f, 10.0f, -11.0f, 12.0f, 
		13.0f, -14.0f, 15.0f, 16.0f);

    mat4f t(
		1.0f, 5.0f, 9.0f, 13.0f, 
		2.0f, 6.0f, 10.0f, -14.0f, 
		3.0f, 7.0f, -11.0f, 15.0f, 
		-4.0f, 8.0f, 12.0f, 16.0f);

    ASSERT_TRUE(t == Transpose(m));
}

TEST(Mat4fTest, TranslationTest) {
    vec4f v(1.0f, 2.0f, 3.0f, 1.0f);
    ASSERT_TRUE(v * Translation(3.0f, -1.0f, 3.0f) == vec4f(4.0f, 1.0f, 6.0f, 1.0f));
}

TEST(Mat4fTest, ScaleTest) {
    vec4f v(1.0f, 2.0f, 3.0f, 1.0f);
    ASSERT_TRUE(v * Scale(2.0f, 3.0f, 5.0f) == vec4f(2.0f, 6.0f, 15.0f, 1.0f));
}

TEST(Mat4fTest, AxisRotationTest) {
    {
        vec4f v(1.0f, 0.0f, 0.0f, 1.0f);

        ASSERT_TRUE(v * RotationX(PIDIV2) == v);
        ASSERT_TRUE(v * RotationY(PIDIV2) == vec4f(0.0f, 0.0f, -1.0f, 1.0f));
        ASSERT_TRUE(v * RotationZ(PIDIV2) == vec4f(0.0f, 1.0f, 0.0f, 1.0f));
    }

    {
        vec4f v(-1.0f, 0.0f, 0.0f, 1.0f);
        ASSERT_TRUE(v * RotationX(PIDIV2) == v);
        ASSERT_TRUE(v * RotationY(PIDIV2) == vec4f(0.0f, 0.0f, 1.0f, 1.0f));
        ASSERT_TRUE(v * RotationZ(PIDIV2) == vec4f(0.0f, -1.0f, 0.0f, 1.0f));
    }

    {
        vec4f v(0.0f, 1.0f, 0.0f, 1.0f);
        ASSERT_TRUE(v * RotationX(PIDIV2) == vec4f(0.0f, 0.0f, 1.0f, 1.0f));
        ASSERT_TRUE(v * RotationY(PIDIV2) == v);
        ASSERT_TRUE(v * RotationZ(PIDIV2) == vec4f(-1.0f, 0.0f, 0.0f, 1.0f));
    }
}

TEST(Mat4fTest, EulerRotationTest) {

    {
        mat4f m = EulerToRotationMatrix(0.0f, 0.0f, 0.0f);
        ASSERT_TRUE(m == mat4f::Identity);
    }

    for(float t = -TWOPI; t <= TWOPI; t += 0.1f) {
        mat4f m   = EulerToRotationMatrix(t, 0.0f, 0.0f);
        auto xm   = RotationX(t);
        auto diff = m - xm;
        ASSERT_TRUE(IsEqual(diff, mat4f::Zero));
    }

    for(float t = -TWOPI; t <= TWOPI; t += 0.1f) {
        mat4f m   = EulerToRotationMatrix(0, t, 0.0f);
        auto xm   = RotationY(t);
        auto diff = m - xm;
        ASSERT_TRUE(IsEqual(diff, mat4f::Zero));
    }

    for(float t = -TWOPI; t <= TWOPI; t += 0.1f) {
        mat4f m   = EulerToRotationMatrix(0, 0, t);
        auto xm   = RotationZ(t);
        auto diff = m - xm;
        ASSERT_TRUE(IsEqual(diff, mat4f::Zero));
    }

    /*#ifdef NDEBUG
            for (float t = -TWOPI; t <= TWOPI; t += 0.1f) {
                    for (float u = -TWOPI; u <= TWOPI; u += 0.1f) {
                            for (float v = -TWOPI; v <= TWOPI; v += 0.1f) {
                                    mat4f m = EulerToRotationMatrix(t, u, v);
                                    auto xm = RotationZ(v) * RotationX(t) * RotationY(u);
                                    ASSERT_TRUE(m == xm);
                            }
                    }
            }
    #else
            for (float t = -TWOPI; t <= TWOPI; t += 0.5f) {
                    for (float u = -TWOPI; u <= TWOPI; u += 0.5f) {
                            for (float v = -TWOPI; v <= TWOPI; v += 0.5f) {
                                    mat4f m = EulerToRotationMatrix(t, u, v);
                                    //auto xm = RotationZ(v) * RotationX(t) * RotationY(u);
                                    auto dxm = DirectX::XMMatrixRotationRollPitchYaw(t, u, v);
                                    if (m != dxm) {
                                            ASSERT_TRUE(true);
                                    }
                                    ASSERT_TRUE(m == dxm);
                            }
                    }
            }
    #endif*/
}

TEST(Mat4fTest, PerspectiveFovTest) {
    mat4f m = PerspectiveFov(Rad(60.0f), 1.3f, 0.01f, 1000.0f);
    auto m2 = DirectX::XMMatrixPerspectiveFovLH(Rad(60.0f), 1.3f, 0.01f, 1000.0f);
    ASSERT_TRUE(m == m2);

    /*#ifdef NDEBUG
            for (float t = 0.1f; t <= TWOPI; t += 0.1f) {
                    for (float ar = 0.5f; ar <= 2.5f; ar += 0.1f) {
                            for (float n = 0.00001f; n <= 1.0f; n += 0.00001f) {
                                    mat4f m = PerspectiveFov(t, ar, n, 1000.0f);
                                    auto xm = DirectX::XMMatrixPerspectiveFovLH(t, ar, n, 1000.0f);
                                    ASSERT_TRUE(IsEqual(m, xm, 0.001f));
                            }
                    }
            }
    #endif */
}

TEST(Mat4fTest, OrthographicTest) {
    const float w = 1231.0f, h = 642.0f, n = 0.0001f, f = 1000.0f;
    mat4f m = Orthographic(w, h, n, f);
    auto m2 = DirectX::XMMatrixOrthographicLH(w, h, n, f);
    ASSERT_TRUE(m == m2);
}

TEST(Mat4fTest, LookAtTest) {
    {
        vec3f p(1.0f, -3.0f, 5.0f);
        vec3f f(0.0f, 0.3f, 0.3f);
        vec3f u(0.0f, 1.0f, 0.0f);
        mat4f m = LookAt(p, f, u);
        auto m2 = DirectX::XMMatrixLookAtLH({p.x, p.y, p.z}, {f.x, f.y, f.z}, {u.x, u.y, u.z});
        ASSERT_TRUE(m == m2);
    }

    {
        vec3f p(1.0f, -3.0f, 5.0f);
        vec3f u(0.0f, 1.0f, 0.0f);
        vec3f f = u;
        mat4f m = LookAt(p, f, u);
        auto m2 = DirectX::XMMatrixLookAtLH({p.x, p.y, p.z}, {f.x, f.y, f.z}, {u.x, u.y, u.z});
        ASSERT_TRUE(m == m2);
    }
}

TEST(Mat4fTest, ToCoordinateSystemTest) {
    vec3f p(1.0f, -3.0f, 5.0f);
    vec3f f(0.0f, 0.3f, 0.3f);
    vec3f u(0.0f, 1.0f, 0.0f);
    mat4f m = ToCoordinateSystem(p, f, u);

    vec4f pos(1.0f, -3.0f, 5.0f, 1.0f);
    auto r = pos * m;
    ASSERT_TRUE(r.xyz() == vec3f::Zero);

    vec4f x = vec4f::UnitX;
}

/*TEST(Mat4fTest, DecomposeTest) {
        vec3f t(90.0f, -102.4f, 12.0f);
        vec3f r(Rad(90.0f), Rad(0.0f), Rad(0.0f));
        vec3f s(1.0f, 1.0f, 1.0f);

        mat4f m = Scale(s) * RotationMatrixFromEulerAngles(r) * Translation(t);

        auto tra = GetTranslation(m);
        ASSERT_TRUE(t == tra);

        auto sca = GetScale(m);
        ASSERT_TRUE(IsEqual(s, sca));

        auto rot = GetRotation(m);
        ASSERT_TRUE(IsRadianEqual(r.x, rot.x) && IsRadianEqual(r.y, rot.y) && IsRadianEqual(r.z,
rot.z));

} */

TEST(Mat4fTest, SRTMatrixTest) {
    auto test = [](const vec3f &s, const vec3f &r, const vec3f &t) {
        auto m   = SRTMatrix(s, r, t);
        auto dxm = DirectX::XMMatrixScaling(s.x, s.y, s.z) *
                   DirectX::XMMatrixRotationRollPitchYaw(r.x, r.y, r.z) *
                   DirectX::XMMatrixTranslation(t.x, t.y, t.z);

        ASSERT_TRUE(m == dxm);
    };

    {
        vec3f t(90.0f, -102.4f, 12.0f);
        vec3f r(Rad(90.0f), Rad(0.0f), Rad(0.0f));
        vec3f s(1.0f, 1.0f, 1.0f);
        test(s, r, t);
    }
}
} // namespace MathUnitTest