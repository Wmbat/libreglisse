#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/maybe.hpp>
#include <libreglisse/result.hpp>

#include <libreglisse/operations/flat_transform_left.hpp>

#include <catch2/catch.hpp>

using namespace reglisse;

auto to_float_either(int i) -> either<float, std::string>
{
   return left(static_cast<float>(i));
}

auto to_string_view_either(const std::string& s) -> either<std::string_view, int>
{
   return left(std::string_view(s.begin(), s.begin() + 2));
}

auto complete_world(std::string&& s) -> either<std::string, int>
{
   s.append(", world");
   return left(std::move(s));
}

SCENARIO("Applying flat transformations on the left element in an either", "[either]")
{
   GIVEN("either's containing a value on the left")
   {
      const either<int, std::string> e_0 = left(0);
      either<std::string, int> e_1 = left("hello");
      either<std::string, int> e_2 = left("hello");

      REQUIRE(e_0.is_left());
      CHECK(e_0 == 0);

      REQUIRE(e_1.is_left());
      CHECK(e_1 == "hello");

      REQUIRE(e_2.is_left());
      CHECK(e_2 == "hello");

      THEN("The value stored within will be modified by the func")
      {
         const either res_0 = e_0 | flat_transform_left(to_float_either);
         const either res_1 = e_1 | flat_transform_left(to_string_view_either);
         const either res_2 = std::move(e_2) | flat_transform_left(complete_world);

         REQUIRE(res_0.is_left());
         CHECK(res_0 == 0.0f);

         REQUIRE(res_1.is_left());
         CHECK(res_1 == "he");

         REQUIRE(res_2.is_left());
         CHECK(res_2 == "hello, world");
      }
   }
   GIVEN("either's containing a value on the right")
   {
      const either<int, std::string> e_0 = right("hello");
      either<std::string, int> e_1 = right(0);
      either<std::string, int> e_2 = right(1);

      REQUIRE(e_0.is_right());
      CHECK(e_0 == "hello");

      REQUIRE(e_1.is_right());
      CHECK(e_1 == 0);

      REQUIRE(e_2.is_right());
      CHECK(e_2 == 1);

      THEN("The value stored within will be not be modified by the func")
      {
         const either res_0 = e_0 | flat_transform_left(to_float_either);
         const either res_1 = e_1 | flat_transform_left(to_string_view_either);
         const either res_2 = std::move(e_2) | flat_transform_left(complete_world);

         REQUIRE(res_0.is_right());
         CHECK(res_0 == "hello");

         REQUIRE(res_1.is_right());
         CHECK(res_1 == 0);

         REQUIRE(res_2.is_right());
         CHECK(res_2 == 1);
      }
   }
}
