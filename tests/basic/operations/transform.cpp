#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/maybe.hpp>
#include <libreglisse/operations/transform.hpp>
#include <libreglisse/result.hpp>

#include <catch2/catch.hpp>

using namespace reglisse;

TEST_CASE("maybe - transform", "[maybe]")
{
   SECTION("Transform pipe on rvalue")
   {
      const auto maybe_int_l = []() -> const maybe<int> {
         return some(1);
      };

      maybe maybe_int = maybe_int_l() | transform([](int val) {
                           return float(val);
                        });

      REQUIRE(maybe_int.is_some());
      CHECK(maybe_int.borrow() == 1.0f);
   }
   SECTION("Transform pipe on rvalue")
   {
      const maybe maybe_float = maybe(some(1)) | transform([](int val) {
                                   return float(val);
                                });

      REQUIRE(maybe_float.is_some());
      CHECK(maybe_float.borrow() == 1.0f);

      const maybe maybe_str = maybe(some(1)) | transform([](int val) {
                                 return std::to_string(val);
                              });

      REQUIRE(maybe_str.is_some());
      CHECK(maybe_str.borrow() == "1");

      const maybe<std::vector<int>> maybe_none = maybe<int>(none) | transform([](int val) {
                                                    return std::vector({val, val});
                                                 });

      CHECK(maybe_none.is_none());
   }
}

enum class err_type
{
   e_fake_err_one,
   e_fake_err_two
};

SCENARIO("Transformation of result's value through pipes", "[result]")
{
   GIVEN("An rvalue result containing a value")
   {
      using res_t = result<std::string, err_type>;

      const result res = res_t(ok(std::string("hello"))) | transform([](std::string&& s) {
                            return s + ", world";
                         });

      THEN("res should hold a value equal to 'hello, world'")
      {
         REQUIRE(res.is_ok());
         CHECK(res.borrow() == "hello, world");
      }
   }
   GIVEN("an rvalue result containing an error")
   {
      using res_t = result<std::string, err_type>;

      const result res = res_t(err(err_type::e_fake_err_one)) | transform([](std::string&& s) {
                            return s + ", world";
                         });

      THEN("res should hold an err_type enum with value 'e_fake_err_one'")
      {
         REQUIRE(res.is_err());
         CHECK(res.borrow_err() == err_type::e_fake_err_one);
      }
   }
   GIVEN("an rvalue result with multiple transform chains")
   {
      using res_t = result<std::string, err_type>;

      // clang-format off
      
      const result res = res_t(ok(std::string("hello"))) 
         | transform([](std::string&& s) { return s + ", world"; }) 
         | transform([](std::string&&) { return std::string(""); });

      // clang-format on

      THEN("res should hold a value equal to the empty string")
      {
         REQUIRE(res.is_ok());
         CHECK(res.borrow() == "");
      }
   }

   GIVEN("an rvalue result transformed into a 'std::vector'")
   {
      using res_t = result<int, err_type>;

      // clang-format off

      const result res = res_t(ok(0)) 
         | transform([](int i) { return std::to_string(i); }) 
         | transform([](std::string&& s) { return std::vector({s, s}); });

      // clang-format on

      THEN("res should contain a vector with 2 strings equal to '0'")
      {
         REQUIRE(res.is_ok());
         CHECK(std::size(res.borrow()) == 2);
         CHECK(res.borrow().at(0) == "0");
         CHECK(res.borrow().at(1) == "0");
      }
   }
}
