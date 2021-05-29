#include <libreglisse/maybe.hpp>

#include <doctest/doctest.h>

#include <vector>

using namespace reglisse;

TEST_SUITE("reglisse::maybe - some test suite") // NOLINT
{
   TEST_CASE("constructor")
   {
      const auto some_int = some<int>(1);
      const auto some_float = some<float>(1.0f);
      const auto some_vector = some<std::vector<int>>({1, 1, 1});

      CHECK(some_int.value() == 1);
      CHECK(some_float.value() == 1.0f);
      CHECK(std::size(some_vector.value()) == 3);
   }

   TEST_CASE("value()")
   {
      SUBCASE("const&")
      {
         const auto some_int = some<int>(1);
         const auto some_float = some<float>(1.0f);
         const auto some_vector = some<std::vector<int>>({1, 1, 1});

         CHECK(some_int.value() == 1);
         CHECK(some_float.value() == 1.0f);
         CHECK(std::size(some_vector.value()) == 3);
      }
      SUBCASE("&")
      {
         auto some_int = some<int>(1);
         auto some_float = some<float>(1.0f);
         auto some_vector = some<std::vector<int>>({1, 1, 1});

         CHECK(some_int.value() == 1);
         CHECK(some_float.value() == 1.0f);
         CHECK(std::size(some_vector.value()) == 3);
      }
      SUBCASE("&&")
      {
         CHECK(some<int>(1).value() == 1);
         CHECK(some<float>(1.0f).value() == 1.0f);
         CHECK(some<std::vector<int>>({1, 1, 1}).value() == std::vector<int>({1, 1, 1}));
      }
   }

   TEST_CASE("comparison - some<T>")
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
   TEST_CASE("comparison - none_t") {}
}
