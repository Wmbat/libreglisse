#include <libreglisse/result.hpp>

#include <catch2/catch.hpp>

#include <vector>

using namespace reglisse;

SCENARIO("result - constructor", "[result]")
{
   GIVEN("A trivial result constructed using ok<T>")
   {
      const result<int, int> trivial(ok(1));

      THEN("The result should contain the value 1")
      {
         REQUIRE(trivial.is_ok());
         CHECK(trivial.borrow() == 1);
      }
   }
   GIVEN("A non-trivial result constructed using ok<T>")
   {
      const result<std::vector<int>, std::string> trivial(ok(std::vector({1, 1, 1})));

      THEN("The result's ok should contain a vector with 3 elements")
      {
         REQUIRE(trivial.is_ok());
         CHECK(trivial.borrow() == std::vector({1, 1, 1}));
      }
   }
   GIVEN("A trivial result constructed using a err<T>")
   {
      const result<int, int> trivial(err(1));

      THEN("The result's should hold 1")
      {
         REQUIRE(trivial.is_err());
         CHECK(trivial.borrow_err() == 1);
      }
   }
   GIVEN("A non-trivial result constructed using err<T>")
   {
      const result<std::vector<int>, std::string> non_trivial(err(std::string("hello")));

      THEN("The result's err should contain a string equal to \"hello\"")
      {
         REQUIRE(non_trivial.is_err());
         CHECK(non_trivial.borrow_err() == "hello");
      }
   }
   GIVEN("A trivial ok result copy constructed from result")
   {
      const result<int, int> trivial = ok(1);
      const result<int, int> trivial_cpy{trivial}; // NOLINT

      THEN("The 2 results should contain the same value")
      {
         REQUIRE(trivial.is_ok());
         REQUIRE(trivial_cpy.is_ok());

         CHECK(trivial.borrow() == 1);
         CHECK(trivial_cpy.borrow() == 1);
      }
   }
   GIVEN("A non-trivial err result copy constructed from result")
   {
      const result<int, std::string> non_trivial = err(std::string("hello"));
      const result<int, std::string> non_trivial_cpy{non_trivial}; // NOLINT

      THEN("The 2 results should contain the same value")
      {
         REQUIRE(non_trivial.is_err());
         REQUIRE(non_trivial_cpy.is_err());

         CHECK(non_trivial.borrow_err() == "hello");
         CHECK(non_trivial_cpy.borrow_err() == "hello");
      }
   }
   GIVEN("A trivial ok result move constructed from result")
   {
      const result trivial{result<int, int>(ok(1))}; // NOLINT

      THEN("The 2 results should contain the same value")
      {
         REQUIRE(trivial.is_ok());
         CHECK(trivial.borrow() == 1);
      }
   }
   GIVEN("A non-trivial err result move constructed from result")
   {
      const result non_trivial{result<int, std::string>(err(std::string("hello")))};

      THEN("The result should contain \"hello\"")
      {
         REQUIRE(non_trivial.is_err());
         CHECK(non_trivial.borrow_err() == "hello");
      }
   }
}

SCENARIO("result - operator=", "[result]")
{
   GIVEN("copies of results")
   {
      const result<int, int> int_result = ok(1);
      const result<int, std::string> str_result = err(std::string("hello"));

      const result dummy_int_result = int_result; // NOLINT
      const result dummy_str_result = str_result; // NOLINT

      THEN("they should be copies of each others")
      {
         REQUIRE(int_result.is_ok());
         CHECK(int_result.borrow() == 1);

         REQUIRE(dummy_int_result.is_ok());
         CHECK(dummy_int_result.borrow() == 1);

         REQUIRE(str_result.is_err());
         CHECK(str_result.borrow_err() == "hello");

         REQUIRE(dummy_str_result.is_err());
         CHECK(dummy_str_result.borrow_err() == "hello");
      }
   }
   GIVEN("results born from move")
   {
      const result int_result = result<int, int>(ok(1));
      const result str_result = result<int, std::string>(err(std::string("hello")));

      THEN("They should be valid & hold the original data")
      {
         REQUIRE(int_result.is_ok());
         CHECK(int_result.borrow() == 1);

         REQUIRE(str_result.is_err());
         CHECK(str_result.borrow_err() == "hello");
      }
   }
}

SCENARIO("result - borrowing", "[result]") {}

SCENARIO("result - taking", "[result]") {}

SCENARIO("result - transforming the data", "[result]") {}
