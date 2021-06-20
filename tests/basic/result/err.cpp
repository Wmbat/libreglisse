#include <libreglisse/result.hpp>

#include <catch2/catch.hpp>

#include <vector>

using namespace reglisse;

TEST_CASE("ok - is ok<T> trivial", "[either][left]")
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

TEST_CASE("ok - constructor", "[either][ok]")
{
   const auto ok_int = ok<int>(1);
   const auto ok_float = ok<float>(1.0f);
   const auto ok_vector = ok<std::vector<int>>({1, 1, 1});

   CHECK(ok_int.value() == 1);
   CHECK(ok_float.value() == 1.0f);
   CHECK(std::size(ok_vector.value()) == 3);
}

TEST_CASE("ok - value()", "[either][ok]")
{
   SECTION("const&")
   {
      const auto ok_int = ok<int>(1);
      const auto ok_float = ok<float>(1.0f);
      const auto ok_vector = ok<std::vector<int>>({1, 1, 1});

      CHECK(ok_int.value() == 1);
      CHECK(ok_float.value() == 1.0f);
      CHECK(std::size(ok_vector.value()) == 3);
   }
   SECTION("&")
   {
      auto ok_int = ok<int>(1);
      auto ok_float = ok<float>(1.0f);
      auto ok_vector = ok<std::vector<int>>({1, 1, 1});

      CHECK(ok_int.value() == 1);
      CHECK(ok_float.value() == 1.0f);
      CHECK(std::size(ok_vector.value()) == 3);
   }
   SECTION("&&")
   {
      CHECK(ok<int>(1).value() == 1);
      CHECK(ok<float>(1.0f).value() == 1.0f);
      CHECK(ok<std::vector<int>>({1, 1, 1}).value() == std::vector<int>({1, 1, 1}));
   }
}

TEST_CASE("ok - comparison")
{
   SECTION("ok<T> comparison with ok<T>")
   {
      const auto ok_int = ok<int>(1);
      const auto ok_float = ok<float>(1.0f);
      const auto ok_vector = ok<std::vector<int>>({1, 1, 1});

      CHECK(ok<int>(0) != ok_int);
      CHECK(ok<float>(0.0f) != ok_float);
      CHECK(ok<std::vector<int>>({1, 1, 0}) != ok_vector);

      CHECK(ok<int>(1) == ok_int);
      CHECK(ok<float>(1.0f) == ok_float);
      CHECK(ok<std::vector<int>>({1, 1, 1}) == ok_vector);
   }
   SECTION("ok<T> comparison with err<T>")
   {
      const auto ok_int = ok<int>(1);
      const auto ok_float = ok<float>(1.0f);
      const auto ok_vector = ok<std::vector<int>>({1, 1, 1});

      CHECK(err<int>(0) != ok_int);
      CHECK(err<float>(0.0f) != ok_float);
      CHECK(err<std::vector<int>>({1, 1, 0}) != ok_vector);

      CHECK(err<int>(1) == ok_int);
      CHECK(err<float>(1.0f) == ok_float);
      CHECK(err<std::vector<int>>({1, 1, 1}) == ok_vector);
   }
}
