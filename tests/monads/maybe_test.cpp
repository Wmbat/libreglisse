#include <monads/maybe.hpp>

/*
#include <gtest/gtest.h>

struct default_struct
{
   default_struct() : i{0} {};

   int i = 0;
};

struct maybe_suite : public testing::Test
{
};

using namespace monad;

TEST_F(maybe_suite, default_ctor)
{
   EXPECT_EQ(noexcept(maybe<int>{}), true);

   const maybe<int> maybe_int{};

   EXPECT_EQ(maybe_int.has_value(), false);
}

TEST_F(maybe_suite, reference_ctor)
{
   {
      int i = 0;
      EXPECT_EQ(noexcept(maybe<int>{i}), true);

      const maybe<int> maybe_int{i};

      EXPECT_EQ(maybe_int.has_value(), true);
      EXPECT_EQ(maybe_int.value(), i);
   }
   {
      const std::string test{"Hello World, this is a magic string."};
      EXPECT_EQ(noexcept(maybe<std::string>{test}), true);

      const maybe<std::string> maybe_str{test};

      EXPECT_EQ(maybe_str.has_value(), true);
      EXPECT_EQ(maybe_str.value(), test);
   }
}

TEST_F(maybe_suite, rvalue_ctor)
{
   {
      EXPECT_EQ(noexcept(maybe<int>{0}), true);

      const maybe<int> maybe_int{0};

      EXPECT_EQ(maybe_int.has_value(), true);
      EXPECT_EQ(maybe_int.value(), 0);
   }
   {
      EXPECT_EQ(noexcept(maybe<std::string>{std::string{"Hello"}}), false);

      const maybe<std::string> maybe_str{std::string{"Hello"}};

      EXPECT_EQ(maybe_str.has_value(), true);
      EXPECT_EQ(maybe_str.value(), "Hello");
   }
}

TEST_F(maybe_suite, pointer_operator)
{
   {
      EXPECT_EQ(noexcept(maybe<default_struct>{default_struct{}}), false);

      const maybe<default_struct> maybe_int{default_struct{}};

      EXPECT_EQ(maybe_int.has_value(), true);
      EXPECT_EQ(maybe_int->i, 0);
   }
   {
      EXPECT_EQ(noexcept(maybe<std::string>{std::string{"Hello"}}), false);

      const maybe<std::string> maybe_str{std::string{"Hello"}};

      EXPECT_EQ(maybe_str.has_value(), true);
      EXPECT_EQ(maybe_str.value(), "Hello");
      EXPECT_EQ(maybe_str->size(), 5);
   }
}

*/
