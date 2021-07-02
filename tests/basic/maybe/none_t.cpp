#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/maybe.hpp>

#include <catch2/catch.hpp>

#include <vector>

using namespace reglisse;

TEST_CASE("is none_t trivial", "[maybe][none_t]")
{
   CHECK(std::is_trivially_copy_assignable_v<none_t>);
   CHECK(std::is_trivially_copy_constructible_v<none_t>);
   CHECK(std::is_trivially_move_assignable_v<none_t>);
   CHECK(std::is_trivially_move_constructible_v<none_t>);
   CHECK(std::is_trivial_v<none_t>);
}

TEST_CASE("none_t comparison with none_t")
{
   const auto my_none = none_t();

   CHECK(my_none == none);
}
TEST_CASE("none_t comparison with some<T>")
{
   const auto some_int = some<int>(0);
   const auto some_float = some<float>(0.0);
   const auto some_vector = some<std::vector<int>>({});

   CHECK(none != some_int);
   CHECK(none != some_float);
   CHECK(none != some_vector);
}
