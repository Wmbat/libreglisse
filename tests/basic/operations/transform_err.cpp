#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/operations/transform.hpp>
#include <libreglisse/operations/transform_err.hpp>

#include <catch2/catch.hpp>

using namespace reglisse;

enum class err_type
{
   e_fake_err_one,
   e_fake_err_two,
};

auto func_with_err()
{
   using res_t = result<std::string, err_type>;

   return res_t(err(err_type::e_fake_err_one));
}

auto complete_world(const std::string& s) -> std::string
{
   return s + ", world!";
}

auto to_string(err_type e) -> std::string
{
   if (e == err_type::e_fake_err_one)
   {
      return "fake_err_one";
   }

   if (e == err_type::e_fake_err_two)
   {
      return "fake_err_two";
   }

   return "unknown";
}

SCENARIO("Transformation of result's error through pipes", "[result]")
{
   GIVEN("A result containing a value")
   {
      using res_t = result<std::string, err_type>;

      const result res = res_t(ok(std::string("hello")));

      REQUIRE(res.is_ok());
      CHECK(res.borrow() == "hello");

      THEN("Nothing should change after `transform_err`")
      {
         const auto new_res = res | transform_err(to_string);

         REQUIRE(new_res.is_ok());
         CHECK(new_res.borrow() == "hello");
      }
   }

   GIVEN("A Result containing an error")
   {
      using res_t = result<std::string, err_type>;

      const result res = res_t(err(err_type::e_fake_err_one));

      REQUIRE(res.is_err());

      THEN("Nothing should change after `transform_err`")
      {
         // clang-format off
         const result new_res = res
            | transform(complete_world) 
            | transform_err(to_string);
         // clang-format on

         REQUIRE(new_res.is_err());
         CHECK(new_res.borrow_err() == to_string(err_type::e_fake_err_one));
      }
   }
}
