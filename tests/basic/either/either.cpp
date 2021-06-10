#include <libreglisse/either.hpp>

#include <catch2/catch.hpp>

#include <vector>

using namespace reglisse;

SCENARIO("either - constructor", "[either]")
{
   GIVEN("A trivial either constructed using left<T>")
   {
      const either<int, int> trivial(left(1));

      THEN("The either's left should contain 1")
      {
         REQUIRE(trivial.is_left());
         CHECK(trivial.borrow_left() == 1);
      }
   }
   GIVEN("A non-trivial either constructed using left<T>")
   {
      const either<std::vector<int>, std::string> trivial(left(std::vector({1, 1, 1})));

      THEN("The either's left should contain a vector with 3 elements")
      {
         REQUIRE(trivial.is_left());
         CHECK(trivial.borrow_left() == std::vector({1, 1, 1}));
      }
   }
   GIVEN("A trivial either constructed using a right<T>")
   {
      const either<int, int> trivial(right(1));

      THEN("The either's right should contain 1")
      {
         REQUIRE(trivial.is_right());
         CHECK(trivial.borrow_right() == 1);
      }
   }
   GIVEN("A non-trivial either constructed using right<T>")
   {
      const either<std::vector<int>, std::string> non_trivial(right(std::string("hello")));

      THEN("The either's right should contain a string equal to \"hello\"")
      {
         REQUIRE(non_trivial.is_right());
         CHECK(non_trivial.borrow_right() == "hello");
      }
   }
   GIVEN("A trivial left either copy constructed from either")
   {
      const either<int, int> trivial = left(1);
      const either<int, int> trivial_cpy{trivial}; // NOLINT

      THEN("The 2 eithers should contain the same value")
      {
         REQUIRE(trivial.is_left());
         REQUIRE(trivial_cpy.is_left());

         CHECK(trivial.borrow_left() == 1);
         CHECK(trivial_cpy.borrow_left() == 1);
      }
   }
   GIVEN("A non-trivial right either copy constructed from either")
   {
      const either<int, std::string> non_trivial = right(std::string("hello"));
      const either<int, std::string> non_trivial_cpy{non_trivial}; // NOLINT

      THEN("The 2 eithers should contain the same value")
      {
         REQUIRE(non_trivial.is_right());
         REQUIRE(non_trivial_cpy.is_right());

         CHECK(non_trivial.borrow_right() == "hello");
         CHECK(non_trivial_cpy.borrow_right() == "hello");
      }
   }
   GIVEN("A trivial left either move constructed from either")
   {
      const either trivial{either<int, int>(left(1))}; // NOLINT

      THEN("The 2 eithers should contain the same value")
      {
         REQUIRE(trivial.is_left());
         CHECK(trivial.borrow_left() == 1);
      }
   }
   GIVEN("A non-trivial right either move constructed from either")
   {
      const either non_trivial{either<int, std::string>(right(std::string("hello")))};

      THEN("The either should contain \"hello\"")
      {
         REQUIRE(non_trivial.is_right());
         CHECK(non_trivial.borrow_right() == "hello");
      }
   }
}

SCENARIO("either - operator=", "[either]")
{
   GIVEN("copies of eithers")
   {
      const either<int, int> int_either = left(1);
      const either<int, std::string> str_either = right(std::string("hello"));

      const either dummy_int_either = int_either; // NOLINT
      const either dummy_str_either = str_either; // NOLINT

      THEN("they should be copies of each others")
      {
         REQUIRE(int_either.is_left());
         CHECK(int_either.borrow_left() == 1);

         REQUIRE(dummy_int_either.is_left());
         CHECK(dummy_int_either.borrow_left() == 1);

         REQUIRE(str_either.is_right());
         CHECK(str_either.borrow_right() == "hello");

         REQUIRE(dummy_str_either.is_right());
         CHECK(dummy_str_either.borrow_right() == "hello");
      }
   }
   GIVEN("eithers born from move")
   {
      const either int_either = either<int, int>(left(1));
      const either str_either = either<int, std::string>(right(std::string("hello")));

      THEN("They should be valid & hold the original data")
      {
         REQUIRE(int_either.is_left());
         CHECK(int_either.borrow_left() == 1);

         REQUIRE(str_either.is_right());
         CHECK(str_either.borrow_right() == "hello");
      }
   }
}

SCENARIO("either - borrowing", "[either]") {}

SCENARIO("either - taking", "[either]") {}

SCENARIO("either - transforming the data", "[either]") {}
