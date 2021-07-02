#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/maybe.hpp>
#include <libreglisse/result.hpp>

#include <libreglisse/operations/and_then.hpp>
#include <libreglisse/operations/transform.hpp>

#include <catch2/catch.hpp>

using namespace reglisse;

namespace maybe_help
{
   auto hello_world(int i) -> maybe<std::string>
   {
      if (i == 0)
      {
         return some("hello, world!");
      }

      return none;
   }

   auto to_ten(int) -> int
   {
      return 10; // NOLINT
   }

   auto to_string(int i)
   {
      return i <= 1 ? maybe(some(std::to_string(i))) : maybe<std::string>(none);
   }

   auto do_nothing(int) { return maybe(some(0)); }

   auto make_empty(const std::string&) { return maybe<int>(none); }
} // namespace maybe_help

SCENARIO("Chaining `and_then` operations on `maybe` monads", "[maybe]")
{
   GIVEN("A simple 'maybe' with a int")
   {
      maybe ex_0 = maybe(some(0));
      maybe ex_1 = maybe(some(1));

      THEN("We should be able to pipe another operation using 'and_then'")
      {
         const maybe hello = ex_0 | and_then(maybe_help::hello_world);

         REQUIRE(hello.is_some());
         CHECK(hello == "hello, world!");

         REQUIRE(ex_0.is_some());
         CHECK(ex_0 == 0);
      }

      THEN("We can double pipe `and_then` with a `transform`")
      {
         const maybe test = ex_1 | transform(maybe_help::to_ten) | and_then(maybe_help::to_string);

         REQUIRE(test.is_none());
      }
   }

   GIVEN("An empty maybe")
   {
      maybe ex_0 = maybe<int>(none);

      THEN("Chaining a `and_then` will not do anthing")
      {
         const maybe result = std::move(ex_0) | and_then(maybe_help::do_nothing);

         REQUIRE(result.is_none());
      }
   }

   GIVEN("A maybe containing 'hello, world'")
   {
      maybe ex_0 = some("hello, world");

      THEN("We can chain it with a 'and_then' that returns and empty monad")
      {
         const maybe res = ex_0 | and_then(maybe_help::make_empty);

         REQUIRE(ex_0.is_some());
         CHECK(ex_0 == "hello, world");

         CHECK(res.is_none());
      }
   }
}

enum class err_type
{
   e_fake_err_0,
   e_fake_err_1,
   e_fake_err_10,
};

namespace result_help
{
   auto int_res(const std::string&) -> result<int, err_type> { return ok(1); }
   auto err_res(const std::vector<int>&) -> result<int, err_type>
   {
      return err(err_type::e_fake_err_10);
   }
} // namespace result_help

SCENARIO("Chaining `and_then` operations on `result` monads", "[result]")
{
   GIVEN("A `result` containing a value")
   {
      result<std::vector<int>, err_type> val_0 = ok(std::vector({1, 1}));
      result<std::string, err_type> val_1 = ok("Hello, world");

      THEN("`and_then` will properly chain the operation into a new monad")
      {
         const result res_0 = val_0 | and_then(result_help::err_res);
         const result res_1 = val_1 | and_then(result_help::int_res);

         REQUIRE(val_0.is_ok());
         CHECK(val_0 == std::vector({1, 1}));
         REQUIRE(val_1.is_ok());
         CHECK(val_1 == std::string{"Hello, world"});

         REQUIRE(res_0.is_err());
         CHECK(res_0 == err_type::e_fake_err_10);
         REQUIRE(res_1.is_ok());
         CHECK(res_1 == 1);
      }
   }

   GIVEN("A `result` containing an error")
   {
      result<std::vector<int>, err_type> err_0 = err(err_type::e_fake_err_0);
      result<std::string, err_type> err_1 = err(err_type::e_fake_err_1);

      THEN("`and_then` will not do anything")
      {
         const result res_0 = err_0 | and_then(result_help::err_res);
         const result res_1 = err_1 | and_then(result_help::int_res);

         REQUIRE(err_0.is_err());
         CHECK(err_0 == err_type::e_fake_err_0);
         REQUIRE(res_0.is_err());
         CHECK(res_0 == err_type::e_fake_err_0);

         REQUIRE(err_1.is_err());
         CHECK(err_1 == err_type::e_fake_err_1);
         REQUIRE(res_1.is_err());
         CHECK(res_1 == err_type::e_fake_err_1);
      }
   }
}
