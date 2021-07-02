#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/operations/transform_left.hpp>
#include <libreglisse/operations/transform_right.hpp>

#include <catch2/catch.hpp>

using namespace reglisse;

SCENARIO("Transformation of either's right through pipes", "[either]")
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

      THEN("`transform_right` will not do anything")
      {
         const either res_0 = e_0 | transform_right([](const std::string& s) {
                                 return std::string_view(s.begin(), s.begin() + 2);
                              });
         const either res_1 = e_1 | transform_right([](int i) {
                                 return static_cast<float>(i);
                              });
         const either res_2 = e_2 | transform_right([](int i) {
                                 return static_cast<float>(i);
                              });

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

      THEN("`transform_right` will modify the content")
      {
         const either res_0 = e_0 | transform_right([](const std::string& s) {
                                 return std::string_view(s.begin(), s.begin() + 2);
                              });
         const either res_1 = e_1 | transform_right([](int i) {
                                 return static_cast<float>(i);
                              });
         const either res_2 = e_2 | transform_right([](int i) {
                                 return static_cast<float>(i);
                              });

         REQUIRE(res_0.is_right());
         CHECK(res_0 == "he");

         REQUIRE(res_1.is_right());
         CHECK(res_1 == 0.0f);

         REQUIRE(res_2.is_right());
         CHECK(res_2 == 1.0f);
      }
   }
}
