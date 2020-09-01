#include <monads/maybe.hpp>
#include <monads/result.hpp>
#include <monads/try.hpp>

#include <exception>
#include <string>
#include <utility>

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
      CHECK(noexcept(maybe<in_place_struct>{in_place, 10, 10}) == true);

      const maybe<in_place_struct> m{in_place, 10, 10};

      REQUIRE(m.has_value() == true);
      CHECK(m->x() == 10);
   }
}

TEST_CASE("result monad test suite")
{
   struct int_field
   {
      int_field() = default;
      int_field(std::uint16_t a, std::uint16_t b, std::uint16_t c, std::uint16_t d) :
         m_a{a}, m_b{b}, m_c{c}, m_d{d}
      {}

      std::uint16_t m_a;
      std::uint16_t m_b;
      std::uint16_t m_c;
      std::uint16_t m_d;
   };

   struct movable_int_field
   {
      movable_int_field() = default;
      movable_int_field(std::uint16_t a, std::uint16_t b, std::uint16_t c, std::uint16_t d) :
         m_a{a}, m_b{b}, m_c{c}, m_d{d}
      {}
      movable_int_field(const movable_int_field&) = delete;
      movable_int_field(movable_int_field&& rhs) noexcept { *this = std::move(rhs); }
      ~movable_int_field() = default;

      auto operator=(const movable_int_field&) = delete;
      auto operator=(movable_int_field&& rhs) noexcept -> movable_int_field&
      {
         std::swap(m_a, rhs.m_a);
         std::swap(m_b, rhs.m_b);
         std::swap(m_c, rhs.m_c);
         std::swap(m_d, rhs.m_d);

         return *this;
      };

      std::uint16_t m_a;
      std::uint16_t m_b;
      std::uint16_t m_c;
      std::uint16_t m_d;
   };

   SUBCASE("value copy ctor")
   {
      {
         int i = 0;

         CHECK(noexcept(result<int, int>{i}) == true);

         result<int, int> test{i};

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == i);
      }
      {
         std::string hello = "hello, world!";

         result<std::string, int> test{hello};

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == hello);
      }
      {
         std::string hello = "hello, world!";
         const auto lambda = [&]() noexcept -> result<std::string, int> {
            return hello;
         };

         const auto test = lambda();

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == hello);
      }
   }
   SUBCASE("value move ctor")
   {
      {
         CHECK(noexcept(result<int, int>{0}) == true);

         result<int, int> test{0};

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == 0);
      }
      {
         result<std::string, int> test{"hello, world!"};

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == "hello, world!");
      }
      {
         const auto lambda = [&]() noexcept -> result<std::string, int> {
            return {"hello, world!"};
         };

         const auto test = lambda();

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == "hello, world!");
      }
      {
         result<std::unique_ptr<int>, int> test{std::make_unique<int>(10)}; // NOLINT

         REQUIRE(test.is_value() == true);
         CHECK(*std::move(test).value().value() == 10);
      }
      {
         const auto lambda = [&]() noexcept -> result<std::unique_ptr<int>, int> {
            return std::make_unique<int>(10); // NOLINT
         };

         auto test = lambda();

         REQUIRE(test.is_value() == true);
         CHECK(*std::move(test).value().value() == 10);
      }
   }
   SUBCASE("value in-place ctor")
   {
      {
         result<int_field, std::error_code> test{in_place, 10, 10, 10, 10}; // NOLINT

         REQUIRE(test.is_value() == true);

         const auto field = test.value();

         CHECK(field->m_a == 10);
         CHECK(field->m_b == 10);
         CHECK(field->m_c == 10);
         CHECK(field->m_d == 10);
      }
      {
         result<movable_int_field, std::error_code> test{in_place, 10, 10, 10, 10}; // NOLINT

         REQUIRE(test.is_value() == true);

         const auto field = std::move(test).value();

         CHECK(field->m_a == 10);
         CHECK(field->m_b == 10);
         CHECK(field->m_c == 10);
         CHECK(field->m_d == 10);
      }
   }

   SUBCASE("error copy ctor")
   {
      {
         int i = 0;

         result<int, int> test{make_error(i)};

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == i);
      }
      {
         const std::string hello = "hello, world!";

         result<int, std::string> test{make_error(hello)};

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == hello);
      }
      {
         std::string hello = "hello, world!";
         const auto lambda = [&](int i) noexcept -> result<int, std::string> {
            if (i > 10)
            {
               return make_error(hello);
            }
            else
            {
               return i;
            }
         };

         const auto test = lambda(11);

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == hello);
      }
   }

   SUBCASE("const l-value map")
   {
      {
         const result<int_field, std::error_code> test{in_place, 0, 1, 2, 3};

         CHECK(test.is_value() == true);

         const auto mapped = test.map([](const int_field& field) {
            return field.m_a + field.m_b + field.m_c + field.m_d;
         });

         REQUIRE(mapped.is_value() == true);

         CHECK(mapped.value().value() == 6);
      }
      {
         const result<int_field, std::error_code> test{in_place, 0, 1, 2, 3};

         CHECK(test.is_value() == true);

         const auto map_fun = [](const int_field& field) {
            return field.m_a + field.m_b + field.m_c + field.m_d;
         };

         const auto mapped = test.map(map_fun);

         REQUIRE(mapped.is_value() == true);

         CHECK(mapped.value().value() == 6);
      }
   }
   SUBCASE("l-value map")
   {
      {
         result<int_field, std::error_code> test{in_place, 0, 1, 2, 3};

         CHECK(test.is_value() == true);

         const auto mapped = test.map([](const int_field& field) {
            return field.m_a + field.m_b + field.m_c + field.m_d;
         });

         REQUIRE(mapped.is_value() == true);

         CHECK(mapped.value().value() == 6);
      }
      {
         result<int_field, std::error_code> test{in_place, 0, 1, 2, 3};

         CHECK(test.is_value() == true);

         const auto map_fun = [](const int_field& field) {
            return field.m_a + field.m_b + field.m_c + field.m_d;
         };

         const auto mapped = test.map(map_fun);

         REQUIRE(mapped.is_value() == true);

         CHECK(mapped.value().value() == 6);
      }
   }
   SUBCASE("const r-value map")
   {
      {
         auto create_result = []() -> const result<int_field, int> {
            return int_field{0, 1, 2, 3};
         };

         auto test = create_result().map([](const int_field&& field) {
            return field.m_a + field.m_b + field.m_c + field.m_d;
         });

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == 6);
      }
      {
         auto create_result = []() -> const result<movable_int_field, int> {
            return movable_int_field{0, 1, 2, 3};
         };

         auto test = create_result().map([](const movable_int_field&& field) {
            return field.m_a + field.m_b + field.m_c + field.m_d;
         });

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == 6);
      }
   }
   SUBCASE("r-value map")
   {
      {
         auto create_result = []() -> result<int_field, int> {
            return int_field{0, 1, 2, 3};
         };

         auto test = create_result().map([](int_field&& field) {
            return field.m_a + field.m_b + field.m_c + field.m_d;
         });

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == 6);
      }
      {
         auto create_result = [](int i) -> result<int_field, int> {
            if (i > 10)
            {
               return int_field{0, 1, 2, 3};
            }
            else
            {
               return make_error(i);
            }
         };

         auto test = create_result(9).map([](int_field&& field) {
            return field.m_a + field.m_b + field.m_c + field.m_d;
         });

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == 9);
      }
      {
         auto create_result = []() -> result<movable_int_field, int> {
            return movable_int_field{0, 1, 2, 3};
         };

         auto test = create_result().map([](movable_int_field&& field) {
            return field.m_a + field.m_b + field.m_c + field.m_d;
         });

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == 6);
      }
      {
         auto create_result = []() -> result<std::unique_ptr<int>, int> {
            return std::make_unique<int>(10); // NOLINT
         };

         auto test = create_result().map([](std::unique_ptr<int>&& ptr) {
            return *ptr;
         });

         REQUIRE(test.is_value() == true);
         CHECK(test.value().value() == 10);
      }
   }
}

TEST_CASE("try_wrap test suite")
{
   {
      auto test = try_wrap<std::exception>([] {
         return std::vector<int>{{1, 2, 3, 4, 5, 6, 7, 8, 9}}; // NOLINT
      });

      REQUIRE(test.is_value() == true);
      CHECK(test.value().value().size() == 9);
   }
   {
      auto test = try_wrap<std::exception>([] {
         throw std::exception{};

         return int{10};
      });

      REQUIRE(test.is_value() == false);
   }
}
