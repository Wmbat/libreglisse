#include <libreglisse/result.hpp>

#include <catch2/catch.hpp>

#include <vector>

using namespace reglisse;

TEST_CASE("err - is err<T> trivial", "[either][right]")
{
   CHECK(std::is_trivially_copy_assignable_v<err<int>>);
   CHECK(std::is_trivially_copy_constructible_v<err<int>>);
   CHECK(std::is_trivially_move_assignable_v<err<int>>);
   CHECK(std::is_trivially_move_constructible_v<err<int>>);
   CHECK_FALSE(std::is_trivial_v<err<int>>); // No default ctor

   using vec_t = err<std::vector<int>>;
   CHECK_FALSE(std::is_trivially_copy_assignable_v<vec_t>);
   CHECK_FALSE(std::is_trivially_copy_constructible_v<vec_t>);
   CHECK_FALSE(std::is_trivially_move_assignable_v<vec_t>);
   CHECK_FALSE(std::is_trivially_move_constructible_v<vec_t>);
   CHECK_FALSE(std::is_trivial_v<vec_t>);
}

TEST_CASE("err - constructor", "[either][err]")
{
   const auto err_int = err<int>(1);
   const auto err_float = err<float>(1.0f);
   const auto err_vector = err<std::vector<int>>({1, 1, 1});

   CHECK(err_int.value() == 1);
   CHECK(err_float.value() == 1.0f);
   CHECK(std::size(err_vector.value()) == 3);
}

TEST_CASE("err - value()", "[either][err]")
{
   SECTION("const&")
   {
      const auto err_int = err<int>(1);
      const auto err_float = err<float>(1.0f);
      const auto err_vector = err<std::vector<int>>({1, 1, 1});

      CHECK(err_int.value() == 1);
      CHECK(err_float.value() == 1.0f);
      CHECK(std::size(err_vector.value()) == 3);
   }
   SECTION("&")
   {
      auto err_int = err<int>(1);
      auto err_float = err<float>(1.0f);
      auto err_vector = err<std::vector<int>>({1, 1, 1});

      CHECK(err_int.value() == 1);
      CHECK(err_float.value() == 1.0f);
      CHECK(std::size(err_vector.value()) == 3);
   }
   SECTION("&&")
   {
      CHECK(err<int>(1).value() == 1);
      CHECK(err<float>(1.0f).value() == 1.0f);
      CHECK(err<std::vector<int>>({1, 1, 1}).value() == std::vector<int>({1, 1, 1}));
   }
}

TEST_CASE("err - comparison")
{
   SECTION("err<T> comparison with err<T>")
   {
      const auto err_int = err<int>(1);
      const auto err_float = err<float>(1.0f);
      const auto err_vector = err<std::vector<int>>({1, 1, 1});

      CHECK(err<int>(0) != err_int);
      CHECK(err<float>(0.0f) != err_float);
      CHECK(err<std::vector<int>>({1, 1, 0}) != err_vector);

      CHECK(err<int>(1) == err_int);
      CHECK(err<float>(1.0f) == err_float);
      CHECK(err<std::vector<int>>({1, 1, 1}) == err_vector);
   }

   SECTION("err<T> comparison with ok<T>")
   {
      const auto err_int = err<int>(1);
      const auto err_float = err<float>(1.0f);
      const auto err_vector = err<std::vector<int>>({1, 1, 1});

      CHECK(ok<int>(0) != err_int);
      CHECK(ok<float>(0.0f) != err_float);
      CHECK(ok<std::vector<int>>({1, 1, 0}) != err_vector);

      CHECK(ok<int>(1) == err_int);
      CHECK(ok<float>(1.0f) == err_float);
      CHECK(ok<std::vector<int>>({1, 1, 1}) == err_vector);
   }
}
