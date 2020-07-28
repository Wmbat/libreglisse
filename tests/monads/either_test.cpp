#include <monads/either.hpp>

#include <gtest/gtest.h>

struct default_struct
{
   default_struct(int i) { data.at(i) = 5; };

   std::array<int, 5> data{};
};

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

      EXPECT_EQ(left_either.is_left(), true);
      EXPECT_EQ(left_either.left().has_value(), true);
      EXPECT_EQ(left_either.left().value(), 0);
   }
   {
      left_t<default_struct> temp{default_struct{0}};
      EXPECT_EQ(noexcept(either<default_struct, std::string>{temp}), true);

      either<default_struct, std::string> left_either{temp};

      EXPECT_EQ(left_either.is_left(), true);
      EXPECT_EQ(left_either.left().has_value(), true);
      EXPECT_EQ(left_either.left().value().data.size(), 5);
   }
}

TEST_F(either_suite, left_move_ctor)
{
   {
      EXPECT_EQ(noexcept(either<int, std::string>{left_t<int>{9}}), true);

      either<int, std::string> left_either{left_t<int>{9}};

      EXPECT_EQ(left_either.is_left(), true);
      EXPECT_EQ(left_either.left().has_value(), true);
      EXPECT_EQ(left_either.left().value(), 9);
   }
   {
      EXPECT_EQ(noexcept(either<default_struct, std::string>{left_t<default_struct>{{0}}}), false);

      either<default_struct, std::string> left_either{left_t<default_struct>{{0}}};

      EXPECT_EQ(left_either.is_left(), true);
      EXPECT_EQ(left_either.left().has_value(), true);
      EXPECT_EQ(left_either.left().value().data.size(), 5);
   }
}
