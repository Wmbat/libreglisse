#include <monads/either.hpp>
#include <monads/maybe.hpp>
#include <monads/result.hpp>
#include <monads/try.hpp>

#include <string>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

using namespace monad;

class in_place_struct
{
public:
   in_place_struct() noexcept = default;
   in_place_struct(int x, int y) noexcept : m_x{x}, m_y{y} {}

   auto x() const noexcept -> int { return m_x; }
   auto y() const noexcept -> int { return m_y; }

private:
   int m_x{0};
   int m_y{0};
};

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

   SUBCASE("in-place constructor")
   {
      CHECK(noexcept(maybe<in_place_struct>{std::in_place, 10, 10}) == true);

      const maybe<in_place_struct> m{std::in_place, 10, 10};

      REQUIRE(m.has_value() == true);
      CHECK(m->x() == 10);
   }
}
