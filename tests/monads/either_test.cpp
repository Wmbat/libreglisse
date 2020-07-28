#include <monads/either.hpp>

#include <gtest/gtest.h>

struct either_suite : public testing::Test
{
};

using namespace monad;

TEST_F(either_suite, left_copy_ctor)
{
   {
      left_t<int> temp{0};
      EXPECT_EQ(noexcept(either<int, std::string>{temp}), true);

      either<int, std::string> left_either{temp};

      EXPECT_EQ(left_either.is_right(), false);
      EXPECT_EQ(left_either.left().has_value(), true);
      EXPECT_EQ(left_either.left().value(), 0);
   }
}

TEST_F(either_suite, left_move_ctor)
{
   {
      EXPECT_EQ(noexcept(either<int, std::string>{left_t<int>{9}}), true);

      either<int, std::string> left_either{left_t<int>{9}};

      EXPECT_EQ(left_either.is_right(), false);
      EXPECT_EQ(left_either.left().has_value(), true);
      EXPECT_EQ(left_either.left().value(), 9);
   }
   {
      either<std::unique_ptr<int>, std::string> e{make_left(std::make_unique<int>(10))};

      EXPECT_EQ(e.is_right(), false);
      EXPECT_EQ(std::move(e).left().has_value(), true);
   }
}
