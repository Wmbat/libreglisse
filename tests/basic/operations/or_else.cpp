#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/maybe.hpp>
#include <libreglisse/result.hpp>

#include <libreglisse/operations/and_then.hpp>
#include <libreglisse/operations/or_else.hpp>
#include <libreglisse/operations/transform.hpp>

#include <catch2/catch.hpp>

using namespace reglisse;

auto maybe_default_int() -> maybe<int>
{
   return some(1);
}

auto maybe_default_string() -> maybe<std::string>
{
   return some("");
}

enum class err_code
{
   e_err_1,
   e_err_2
};

auto result_value_int(err_code) -> result<int, err_code>
{
   return ok(1);
}
auto result_error_int(err_code) -> result<int, err_code>
{
   return err(err_code::e_err_1);
}

auto result_value_str(err_code) -> result<std::string, err_code>
{
   return ok("");
}
auto result_error_str(err_code) -> result<std::string, err_code>
{
   return err(err_code::e_err_2);
}

SCENARIO("'or_else' on maybe monads", "[maybe]")
{
   GIVEN("a maybe containing a value")
   {
      maybe ex_0 = some(0);
      maybe ex_1 = some("hello, world. This string excedes the SSBO optimization");

      CHECK(ex_0.is_some());
      CHECK(ex_0 == 0);

      CHECK(ex_1.is_some());
      CHECK(ex_1 == "hello, world. This string excedes the SSBO optimization");

      THEN("Nothing will happen when 'or_else' is applied")
      {
         const maybe res_0 = ex_0 | or_else(maybe_default_int);

         CHECK(res_0.is_some());
         CHECK(res_0 == 0);

         const maybe res_1 = ex_1 | or_else(maybe_default_string);

         CHECK(res_1.is_some());
         CHECK(res_1 == "hello, world. This string excedes the SSBO optimization");
      }
   }

   GIVEN("a maybe not containing any value")
   {
      maybe<int> ex_0 = none;
      maybe<std::string> ex_1 = none;

      CHECK(ex_0.is_none());
      CHECK(ex_1.is_none());

      THEN("Nothing will happen when 'or_else' is applied")
      {
         const maybe res_0 = ex_0 | or_else(maybe_default_int);

         CHECK(res_0.is_some());
         CHECK(res_0 == 1);

         const maybe res_1 = ex_1 | or_else(maybe_default_string);

         CHECK(res_1.is_some());
         CHECK(res_1 == "");
      }
   }
}

SCENARIO("'or_else' on result monads", "[result]")
{
   GIVEN("A result containing a value")
   {
      const result<int, err_code> ex_0 = ok(0);
      result<std::string, err_code> ex_1 =
         ok("hello, world. This string excedes the SSBO optimization");

      CHECK(ex_0.is_ok());
      CHECK(ex_1.is_ok());

      THEN("Nothing will happen when 'or_else' is applied")
      {
         const auto res_0 = ex_0 | or_else(result_value_int);
         const auto res_1 = result<int, err_code>(ok(0)) | or_else(result_error_int);
         const auto res_2 = ex_1 | or_else(result_value_str);
         const auto res_3 = ex_1 | or_else(result_error_str);

         CHECK(res_0.is_ok());
         CHECK(res_0 == ex_0);

         CHECK(res_1.is_ok());
         CHECK(res_1 == ex_0);

         CHECK(res_2.is_ok());
         CHECK(res_2 == ex_1);

         CHECK(res_3.is_ok());
         CHECK(res_3 == ex_1);
      }
   }
   GIVEN("A result containing an error")
   {
      const result<int, err_code> ex_0 = err(err_code::e_err_1);
      result<std::string, err_code> ex_1 = err(err_code::e_err_2);

      CHECK(ex_0.is_err());
      CHECK(ex_1.is_err());

      THEN("The 'or_else' will modify the value stored")
      {
         const auto res_0 = ex_0 | or_else(result_value_int);
         const auto res_1 =
            result<int, err_code>(err(err_code::e_err_1)) | or_else(result_error_int);
         const auto res_2 = ex_1 | or_else(result_value_str);
         const auto res_3 = ex_1 | or_else(result_error_str);

         CHECK(res_0.is_ok());
         CHECK(res_0 == 1);

         CHECK(res_1.is_err());
         CHECK(res_1 == err_code::e_err_1);

         CHECK(res_2.is_ok());
         CHECK(res_2 == std::string(""));

         CHECK(res_3.is_err());
         CHECK(res_3 == err_code::e_err_2);
      }
   }
}
