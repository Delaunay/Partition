#include <Utilities/Memory.h>
#include <gtest/gtest.h>

using pn::pool_allocator;

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

TEST(PoolAllocatorTest, ConstructorTest) {
    {
        pool_allocator<char> a(5);
        ASSERT_TRUE(a.HasFree());
    }

    {
        pool_allocator<int> a(50);
        ASSERT_TRUE(a.HasFree());
    }

    {
        pool_allocator<blob> a(1024);
        ASSERT_TRUE(a.HasFree());
    }
}

TEST(PoolAllocatorTest, CreateAndDestroyTest) {
    pool_allocator<blob> a(2);

    blob *b = a.Create();
    ASSERT_TRUE(b != nullptr);
    ASSERT_TRUE(a.HasFree());

    blob *c = a.Create(1, 2, 3, 10.4, 15.4f);
    ASSERT_TRUE(c != nullptr);
    ASSERT_FALSE(a.HasFree());
    ASSERT_EQ(1, c->i);
    ASSERT_EQ(2, c->j);
    ASSERT_EQ(3, c->k);
    ASSERT_EQ(10.4, c->d);
    ASSERT_EQ(15.4f, c->f);

    blob *d = a.Create();
    ASSERT_TRUE(d == nullptr);
    ASSERT_FALSE(a.HasFree());

    a.Release(c);
    ASSERT_TRUE(a.HasFree());
}
} // namespace MemoryUnitTest