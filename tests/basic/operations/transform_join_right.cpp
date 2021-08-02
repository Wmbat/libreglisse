#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/maybe.hpp>
#include <libreglisse/result.hpp>

#include <libreglisse/operations/transform_join_right.hpp>

#include <catch2/catch.hpp>

using namespace reglisse;

auto to_string_view_either_r(const std::string& s) -> either<int, std::string_view>
{
   return right(std::string_view(s.begin(), s.begin() + 2));
}

auto to_float(int i) -> either<std::string, float>
{
   return right(static_cast<float>(i));
}

SCENARIO("Applying flat transformations on the right element in an either", "[either]")
{
   GIVEN("Either's containing a left value")
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

      THEN("The value stored within will be not be modified by the func")
      {
         const either res_0 = e_0 | transform_join_right(to_string_view_either_r);
         const either res_1 = e_1 | transform_join_right(to_float);
         const either res_2 = e_2 | transform_join_right(to_float);

         REQUIRE(res_0.is_left());
         CHECK(res_0 == 0);

         REQUIRE(res_1.is_left());
         CHECK(res_1 == "hello");

         REQUIRE(res_2.is_left());
         CHECK(res_2 == "hello");
      }
   }

   GIVEN("Either's containing a right value")
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

      THEN("The value stored within will be modified by the func")
      {
         const either res_0 = e_0 | transform_join_right(to_string_view_either_r);
         const either res_1 = e_1 | transform_join_right(to_float);
         const either res_2 = e_2 | transform_join_right(to_float);

         REQUIRE(res_0.is_right());
         CHECK(res_0 == "he");

         REQUIRE(res_1.is_right());
         CHECK(res_1 == 0.0f);

         REQUIRE(res_2.is_right());
         CHECK(res_2 == 1.0f);
      }
   }
}
