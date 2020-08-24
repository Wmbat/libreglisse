#include <monads/maybe.hpp>
#include <monads/either.hpp>
#include <monads/result.hpp>
#include <monads/try.hpp>

#include <string>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

using namespace monad;

TEST_CASE("maybe monad test suite")
{
   SUBCASE("Default constructor")
   {
      CHECK(noexcept(maybe<int>{}) == true);

      const maybe<int> m{};

      CHECK(m.has_value() == false);
   }

   SUBCASE("const reference based constructor")
   {
      {
         int i = 0;
         CHECK(noexcept(maybe<int>{i}) == true);

         const maybe<int> m{i};

         REQUIRE(m.has_value() == true);
         CHECK(m.value() == 0);
      }
      {
         const std::string test{"Hello World, this is a magic string."};
         CHECK(noexcept(maybe<std::string>{test}) == true);

         const maybe<std::string> maybe_str{test};

         REQUIRE(maybe_str.has_value() == true);
         CHECK(maybe_str.value() == test);
      }
   }

   SUBCASE("rvalue based constructor")
   {
      {
         CHECK(noexcept(maybe<int>{0}) == true);

         const maybe<int> maybe_int{0};

         REQUIRE(maybe_int.has_value() == true);
         CHECK(maybe_int.value() == 0);
      }
      {
         CHECK(noexcept(maybe<std::string>{std::string{"Hello"}}) == false);

         const maybe<std::string> maybe_str{std::string{"Hello"}};

         REQUIRE(maybe_str.has_value() == true);
         CHECK(maybe_str.value() == "Hello");
      }
   }
}
