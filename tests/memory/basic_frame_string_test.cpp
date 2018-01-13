#include <Utilities/UtilityTypes.h>
#include <Utilities/frame_string.h>
#include <gtest/gtest.h>

using pn::frame_string;
using pn::linear_allocator;

namespace MemoryUnitTest {
TEST(BasicFrameStringTest, ConstructorTest) {
    linear_allocator a(5);
    frame_string::SetFrameAllocator(&a);

    {
        // default constructor
        frame_string s;
        ASSERT_TRUE(s.data == nullptr);
        ASSERT_TRUE(s.length == 0);
        ASSERT_TRUE(a.HasFree());
        ASSERT_TRUE(a.HasFree(5));

        // const char* constructor
        frame_string s2("Hello");
        ASSERT_TRUE(s2.data != nullptr);
        ASSERT_TRUE(s2.length == 5);
        ASSERT_FALSE(a.HasFree());
        ASSERT_EQ('H', s2[0]);
        ASSERT_EQ('e', s2[1]);
        ASSERT_EQ('l', s2[2]);
        ASSERT_EQ('l', s2[3]);
        ASSERT_EQ('o', s2[4]);

        // move constructor
        frame_string s3(std::move(s2));
        ASSERT_TRUE(s2.data == nullptr);
        ASSERT_TRUE(s2.length == 0);

        ASSERT_TRUE(s3.data != nullptr);
        ASSERT_TRUE(s3.length == 5);
        ASSERT_FALSE(a.HasFree());
        ASSERT_EQ('H', s3[0]);
        ASSERT_EQ('e', s3[1]);
        ASSERT_EQ('l', s3[2]);
        ASSERT_EQ('l', s3[3]);
        ASSERT_EQ('o', s3[4]);
    }

    a.Release();

    {
        frame_string s("ba");
        ASSERT_TRUE(a.HasFree(3));

        // copy constructor
        frame_string s2(s);
        ASSERT_TRUE(a.HasFree(1));
        ASSERT_TRUE(s[0] == s2[0]);
        ASSERT_TRUE(s[1] == s2[1]);
    }
}

TEST(BasicFrameStringTest, EqualityTest) {
    linear_allocator a(1024);
    frame_string::SetFrameAllocator(&a);

    frame_string s("this is a long string /n and it's a lot of words...");
    auto         s2 = s;

    ASSERT_TRUE(s == "this is a long string /n and it's a lot of words...");

    ASSERT_TRUE(s == s2);
    ASSERT_FALSE(s != s2);

    frame_string s3("this is a string.");
    ASSERT_FALSE(s == s3);
    ASSERT_TRUE(s != s3);

    std::string stds("this is a string.");
    ASSERT_TRUE(stds == s3);
}

TEST(BasicFrameStringTest, EmptyStringTest) {
    linear_allocator a(1024);
    frame_string::SetFrameAllocator(&a);

    frame_string s;
    frame_string s2("");

    ASSERT_TRUE(s == s2);
    ASSERT_TRUE(s == "");
    ASSERT_TRUE(0 == s.length);
    ASSERT_TRUE(0 == s2.length);
}

TEST(BasicFrameStringTest, ConcatenationTest) {
    linear_allocator a(20);
    frame_string::SetFrameAllocator(&a);

    {
        frame_string s("thisa");
        frame_string s2("THISA");
        auto         b = s + s2;

        ASSERT_TRUE(b == "thisaTHISA");
        ASSERT_FALSE(a.HasFree());
    }

    a.Release();

    {
        frame_string s("thisa");
        auto         b = "THISA" + s;

        ASSERT_TRUE(b == "THISAthisa");
        ASSERT_TRUE(a.HasFree(5));
    }

    a.Release();

    {
        frame_string s("thisa");
        pn::string   s2("THISA");
        auto         b = s + s2;

        ASSERT_TRUE(b == "thisaTHISA");
        ASSERT_TRUE(a.HasFree(5));
    }
}
} // namespace MemoryUnitTest