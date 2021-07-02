#define LIBREGLISSE_USE_EXCEPTIONS

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

SCENARIO("result - borrowing", "[result]")
{
   GIVEN("Results containing a value")
   {
      const result<int, int> r_0 = ok(1);
      result<int, std::string> r_1 = ok(0);

      THEN("Borrowing will give access to the value stored")
      {
         CHECK(r_0.borrow() == 1);
         CHECK(r_1.borrow() == 0);
      }
      THEN("borrowing the error stored will throw an exception")
      {
         CHECK_THROWS(r_0.borrow_err() == 0);
         CHECK_THROWS(r_1.borrow_err() == "hello");
      }
   }
   GIVEN("Results containing an error")
   {
      const result<int, int> r_0 = err(1);
      result<int, std::string> r_1 = err("hello");

      THEN("Borrowing the value stored will throw an exception")
      {
         CHECK_THROWS(r_0.borrow() == 1);
         CHECK_THROWS(r_1.borrow() == 0);
      }
      THEN("borrowing the error stored will work fine")
      {
         CHECK(r_0.borrow_err() == 1);
         CHECK(r_1.borrow_err() == "hello");
      }
   }
}

SCENARIO("result - taking", "[result]")
{
   GIVEN("Results containing a value")
   {
      result<int, int> r_0 = ok(1);
      result<int, std::string> r_1 = ok(0);

      result<int, int> r_2 = ok(1);
      result<int, std::string> r_3 = ok(0);

      THEN("Borrowing will give access to the value stored")
      {
         CHECK(std::move(r_0).take() == 1);
         CHECK(std::move(r_1).take() == 0);
      }
      THEN("borrowing the error stored will throw an exception")
      {
         CHECK_THROWS(std::move(r_2).take_err() == 0);
         CHECK_THROWS(std::move(r_3).take_err() == "hello");
      }
   }
   GIVEN("Results containing an error")
   {
      result<int, int> r_0 = err(1);
      result<int, std::string> r_1 = err("hello");

      result<int, int> r_2 = err(1);
      result<int, std::string> r_3 = err("hello");

      THEN("taking the value stored will throw an exception")
      {
         CHECK_THROWS(std::move(r_0).borrow() == 1);
         CHECK_THROWS(std::move(r_1).borrow() == 0);
      }
      THEN("taking the error stored will work fine")
      {
         CHECK(std::move(r_2).take_err() == 1);
         CHECK(std::move(r_3).take_err() == "hello");
      }
   }
}
