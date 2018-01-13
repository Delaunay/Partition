#include <Utilities/Memory.h>
#include <gtest/gtest.h>

using pn::linear_allocator;

namespace MemoryUnitTest {
struct blob {
    int    i;
    int    j;
    int    k;
    double d;
    float  f;
    blob() {}
    blob(int i, int j, int k, double d, float f) : i(i), j(j), k(k), d(d), f(f) {}
};

TEST(LinearAllocatorTest, ConstructorTest) {
    linear_allocator a(1024);
    ASSERT_TRUE(a.HasFree(1024));
    ASSERT_TRUE(a.HasFree());
}

TEST(LinearAllocatorTest, CreateAndDestroyTest) {
    linear_allocator a(16);

    auto *b = a.Allocate(4);
    ASSERT_FALSE(a.HasFree(16));
    ASSERT_TRUE(a.HasFree(12));

    int *i = a.Create<int>(5);
    ASSERT_EQ(5, *i);
    ASSERT_TRUE(a.HasFree(8));

    auto *c = a.Allocate(8);
    ASSERT_FALSE(a.HasFree());

    a.Release();
    ASSERT_TRUE(a.HasFree());
    ASSERT_TRUE(a.HasFree(16));
}
} // namespace MemoryUnitTest