#include "CppUnitTest.h"
#include "stdafx.h"

#include <Utilities/Math.h>
#include <gtest/gtest.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace pn;

namespace MathUnitTest {
TEST(Vec3fTest, SizeTest) { ASSERT_FLOAT_EQ(static_cast<size_t>(12), sizeof(vec3f)); }

TEST(Vec3fTest, CrossProductTest) {
    vec3f u(3.0f, 1.0f, -1.0f);
    vec3f v(2.0f, 3.0f, 0.0f);

    ASSERT_TRUE(vec3f(3, -2, 7) == Cross(u, v));
    ASSERT_TRUE(vec3f::Zero == Cross(u, u));
}
} // namespace MathUnitTest