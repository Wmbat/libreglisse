#define LIBREGLISSE_USE_EXCEPTIONS

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

SCENARIO("either - borrowing", "[either]")
{
   GIVEN("either containing a left value")
   {
      const either<int, int> e_0 = left(1);
      either<int, std::string> e_1 = left(0);

      THEN("Borrowing will give access to the left value stored")
      {
         CHECK(e_0.borrow_left() == 1);
         CHECK(e_1.borrow_left() == 0);
      }
      THEN("borrowing the right value will throw an exception")
      {
         // CHECK_THROWS(e_0.borrow_right() == 0);
         // CHECK_THROWS(e_1.borrow_right() == "hello");
      }
   }
   GIVEN("either containing a right value")
   {
      const either<int, int> e_0 = right(1);
      either<int, std::string> e_1 = right("hello");

      THEN("Borrowing the left value will throw an exception")
      {
         // CHECK_THROWS(e_0.borrow_left() == 1);
         // CHECK_THROWS(e_1.borrow_left() == 0);
      }
      THEN("borrowing the right value will work fine")
      {
         CHECK(e_0.borrow_right() == 1);
         CHECK(e_1.borrow_right() == "hello");
      }
   }
}

SCENARIO("either - taking", "[either]")
{
   GIVEN("either containing a left value")
   {
      either<int, int> e_0 = left(1);
      either<int, std::string> e_1 = left(0);

      either<int, int> e_2 = left(1);
      either<int, std::string> e_3 = left(0);

      THEN("Borrowing will give access to the left value stored")
      {
         CHECK(std::move(e_0).take_left() == 1);
         CHECK(std::move(e_1).take_left() == 0);
      }
      THEN("borrowing the right value will throw an exception")
      {
         // CHECK_THROWS(e_0.borrow_right() == 0);
         // CHECK_THROWS(e_1.borrow_right() == "hello");
      }
   }
   GIVEN("either containing a right value")
   {
      either<int, int> e_0 = right(1);
      either<int, std::string> e_1 = right("hello");

      either<int, int> e_2 = right(1);
      either<int, std::string> e_3 = right("hello");

      THEN("Borrowing the left value will throw an exception")
      {
         // CHECK_THROWS(e_0.borrow_left() == 1);
         // CHECK_THROWS(e_1.borrow_left() == 0);
      }
      THEN("borrowing the right value will work fine")
      {
         CHECK(std::move(e_2).take_right() == 1);
         CHECK(std::move(e_3).take_right() == "hello");
      }
   }
}
