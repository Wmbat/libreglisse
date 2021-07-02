#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/maybe.hpp>

#include <catch2/catch.hpp>

#include <memory>
#include <vector>

using namespace reglisse;

TEST_CASE("some - is some<T> trivial", "[maybe][some]")
{
   CHECK(std::is_trivially_copy_assignable_v<some<int>>);
   CHECK(std::is_trivially_copy_constructible_v<some<int>>);
   CHECK(std::is_trivially_move_assignable_v<some<int>>);
   CHECK(std::is_trivially_move_constructible_v<some<int>>);
   CHECK_FALSE(std::is_trivial_v<some<int>>); // No default ctor

   using vec_t = some<std::vector<int>>;
   CHECK_FALSE(std::is_trivially_copy_assignable_v<vec_t>);
   CHECK_FALSE(std::is_trivially_copy_constructible_v<vec_t>);
   CHECK_FALSE(std::is_trivially_move_assignable_v<vec_t>);
   CHECK_FALSE(std::is_trivially_move_constructible_v<vec_t>);
   CHECK_FALSE(std::is_trivial_v<vec_t>);
}

TEST_CASE("some(value_type&&)", "[maybe][some]")
{
   SECTION("trivial type")
   {
      const auto int_val = some(1);
      const auto float_val = some(1.0f);

      CHECK(int_val.value() == 1);
      CHECK(float_val.value() == 1.0f);
   }
   SECTION("non-trivial")
   {
      const auto vec_val = some(std::vector({1, 1, 1}));

      CHECK(std::size(vec_val.value()) == 3);
      for (int val : vec_val.value())
      {
         CHECK(val == 1);
      }

      const auto ptr_val = some(std::make_unique<int>(0));

      CHECK(*ptr_val.value() == 0);
   }
}

TEST_CASE("some::value()", "[maybe][some]")
{
   SECTION("const&")
   {
      const auto some_int = some<int>(1);
      const auto some_float = some<float>(1.0f);
      const auto some_vector = some<std::vector<int>>({1, 1, 1});

      CHECK(some_int.value() == 1);
      CHECK(some_float.value() == 1.0f);
      CHECK(std::size(some_vector.value()) == 3);
   }
   SECTION("&")
   {
      auto some_int = some<int>(1);
      auto some_float = some<float>(1.0f);
      auto some_vector = some<std::vector<int>>({1, 1, 1});

      CHECK(some_int.value() == 1);
      CHECK(some_float.value() == 1.0f);
      CHECK(std::size(some_vector.value()) == 3);
   }
   SECTION("&&")
   {
      CHECK(some<int>(1).value() == 1);
      CHECK(some<float>(1.0f).value() == 1.0f);
      CHECK(some<std::vector<int>>({1, 1, 1}).value() == std::vector<int>({1, 1, 1}));
   }
}

TEST_CASE("some<T> comparison with some<T>")
{
   const auto some_int = some<int>(1);
   const auto some_float = some<float>(1.0f);
   const auto some_vector = some<std::vector<int>>({1, 1, 1});

   CHECK(some<int>(0) != some_int);
   CHECK(some<float>(0.0f) != some_float);
   CHECK(some<std::vector<int>>({1, 1, 0}) != some_vector);

   CHECK(some<int>(1) == some_int);
   CHECK(some<float>(1.0f) == some_float);
   CHECK(some<std::vector<int>>({1, 1, 1}) == some_vector);
}

TEST_CASE("some<T> comparison with none_t")
{
   const auto some_int = some<int>(1);
   const auto some_float = some<float>(1.0f);
   const auto some_vector = some<std::vector<int>>({1, 1, 1});

   CHECK(none != some_int);
   CHECK(none != some_float);
   CHECK(none != some_vector);
}
