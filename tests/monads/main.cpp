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

   [[nodiscard]] auto x() const noexcept -> int { return m_x; }
   [[nodiscard]] auto y() const noexcept -> int { return m_y; }

private:
   int m_x{0};
   int m_y{0};
};

TEST_SUITE("maybe test suite")
{
   TEST_CASE("Default constructor")
   {
      CHECK(noexcept(maybe<int>{}) == true);

      const maybe<int> m{};

      CHECK(m.has_value() == false);
   }

   TEST_CASE("const reference based constructor")
   {
      SUBCASE("trivial")
      {
         int i = 0;
         CHECK(noexcept(maybe<int>{i}) == true);

         const maybe<int> m{i};

         REQUIRE(m.has_value() == true);
         CHECK(m.value() == 0);
      }

      SUBCASE("non-trivial")
      {
         const std::string test{"Hello World, this is a magic string."};
         CHECK(noexcept(maybe<std::string>{test}) == true);

         const maybe<std::string> maybe_str{test};

         REQUIRE(maybe_str.has_value() == true);
         CHECK(maybe_str.value() == test);
      }
   }

   TEST_CASE("rvalue based constructor")
   {
      SUBCASE("trivial")
      {
         CHECK(noexcept(maybe<int>{0}) == true);

         const maybe<int> maybe_int{0};

         REQUIRE(maybe_int.has_value() == true);
         CHECK(maybe_int.value() == 0);
      }

      SUBCASE("non-trivial")
      {
         CHECK(noexcept(maybe<std::string>{std::string{"Hello"}}) == false);

         const maybe<std::string> maybe_str{std::string{"Hello"}};

         REQUIRE(maybe_str.has_value() == true);
         CHECK(maybe_str.value() == "Hello");
      }
   }

   TEST_CASE("in-place constructor")
   {
      SUBCASE("trivial")
      {
         CHECK(noexcept(maybe<in_place_struct>{in_place, 10, 10}) == true);

         const maybe<in_place_struct> m{in_place, 10, 10};

         REQUIRE(m.has_value() == true);
         CHECK(m->x() == 10);
      }

      SUBCASE("non-trivial")
      {
         using my_tuple = std::tuple<int, std::string, in_place_struct>;

         CHECK(noexcept(maybe<my_tuple>{in_place, 10, "hello", in_place_struct{}}) == false);

         const maybe<my_tuple> m{in_place, 10, "hello", in_place_struct{}};

         REQUIRE(m.has_value() == true);
      }
   }

   TEST_CASE("copy ctor")
   {
      SUBCASE("trivial")
      {
         maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == 10);

         maybe<int> my_new_maybe{my_maybe};

         REQUIRE(my_new_maybe.has_value());
         CHECK(my_new_maybe.value() == 10);

         CHECK(my_maybe == my_new_maybe);
      }
      SUBCASE("non-trivial")
      {
         maybe<std::string> my_temp_maybe{"hello"};

         REQUIRE(my_temp_maybe.has_value());
         CHECK(my_temp_maybe.value() == std::string{"hello"});

         const maybe<std::string> my_maybe{my_temp_maybe};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == std::string{"hello"});

         CHECK(my_temp_maybe == my_maybe);
      }
   }
   TEST_CASE("move ctor")
   {
      SUBCASE("trivial")
      {
         maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == 10);

         maybe<int> my_new_maybe{std::move(my_maybe)}; // since trivial, move has no effect

         CHECK(my_maybe.has_value());

         REQUIRE(my_new_maybe.has_value());
         CHECK(my_new_maybe.value() == 10);
      }
      SUBCASE("non-trivial")
      {
         {
            maybe<std::string> my_temp_maybe{"hello"};

            REQUIRE(my_temp_maybe.has_value());
            CHECK(my_temp_maybe.value() == std::string{"hello"});

            const maybe<std::string> my_maybe{std::move(my_temp_maybe)};

            REQUIRE(!my_temp_maybe.has_value());
            REQUIRE(my_maybe.has_value());
            CHECK(my_maybe.value() == std::string{"hello"});
         }
         {
            maybe<std::unique_ptr<int>> my_maybe{
               maybe<std::unique_ptr<int>>{std::make_unique<int>(10)}};

            REQUIRE(my_maybe.has_value());
            CHECK(*my_maybe.value() == 10);
         }
      }
   }

   TEST_CASE("copy assignment operator")
   {
      SUBCASE("trivial")
      {
         const maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == 10);

         const maybe my_new_maybe = my_maybe;

         REQUIRE(my_new_maybe.has_value());
         CHECK(my_new_maybe.value() == 10);

         CHECK(my_maybe == my_new_maybe);
      }
      SUBCASE("non-trivial")
      {
         const maybe<std::string> my_temp_maybe{"hello"};

         REQUIRE(my_temp_maybe.has_value());
         CHECK(my_temp_maybe.value() == std::string{"hello"});

         const maybe my_maybe = my_temp_maybe;

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == std::string{"hello"});

         CHECK(my_temp_maybe == my_maybe);
      }
   }
   TEST_CASE("move assignment operator")
   {
      SUBCASE("trivial")
      {
         maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == 10);

         maybe my_new_maybe = std::move(my_maybe); // since trivial, move has no effect

         CHECK(my_maybe.has_value());

         REQUIRE(my_new_maybe.has_value());
         CHECK(my_new_maybe.value() == 10);
      }
      SUBCASE("non-trivial")
      {
         {
            maybe<std::string> my_temp_maybe{"hello"};

            REQUIRE(my_temp_maybe.has_value());
            CHECK(my_temp_maybe.value() == std::string{"hello"});

            const maybe my_maybe = std::move(my_temp_maybe);

            REQUIRE(my_maybe.has_value());
            CHECK(my_maybe.value() == std::string{"hello"});
         }
         {
            maybe my_maybe = maybe<std::unique_ptr<int>>{std::make_unique<int>(10)};

            REQUIRE(my_maybe.has_value());
            CHECK(*my_maybe.value() == 10);
         }
      }
   }

   TEST_CASE("operator->")
   {
      struct test
      {
         int x;
         int y;
      };

      SUBCASE("const")
      {
         const maybe<test> const_test_maybe{test{.x = 10, .y = 20}};

         REQUIRE(const_test_maybe.has_value() == true);
         CHECK(const_test_maybe->x == 10);
         CHECK(const_test_maybe->y == 20);
      }

      SUBCASE("non-const")
      {
         maybe<test> trivial_maybe{test{.x = 10, .y = 20}};

         REQUIRE(trivial_maybe.has_value() == true);
         CHECK(trivial_maybe->x == 10);
         CHECK(trivial_maybe->y == 20);

         trivial_maybe->x = 20;

         CHECK(trivial_maybe->x == 20);
      }
   }

   TEST_CASE("operator* const l-value")
   {
      SUBCASE("trivial")
      {
         const maybe<in_place_struct> m{in_place, 10, 10};

         const auto my_struct = *m;

         CHECK(my_struct.x() == 10);
         CHECK(my_struct.y() == 10);
      }
      SUBCASE("non-trivial")
      {
         const maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe);

         const std::string str = *my_maybe;

         CHECK(str == "hello");
      }
   }
   TEST_CASE("operator* l-value")
   {
      SUBCASE("trivial")
      {
         maybe<in_place_struct> m{in_place, 10, 10};

         auto& my_struct = *m;

         CHECK(my_struct.x() == 10);
         CHECK(my_struct.y() == 10);
      }
      SUBCASE("non-trivial")
      {
         maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe);

         std::string& str = *my_maybe;

         CHECK(str == "hello");
      }
   }
   TEST_CASE("operator* const r-value")
   {
      SUBCASE("trivial")
      {
         auto my_struct = *maybe<in_place_struct>{in_place, 10, 10};

         CHECK(my_struct.x() == 10);
         CHECK(my_struct.y() == 10);
      }
      SUBCASE("non-trivial")
      {
         std::string str = *maybe<std::string>{"hello"};

         CHECK(str == "hello");
      }
   }
   TEST_CASE("operator* r-value")
   {
      SUBCASE("trivial")
      {
         auto my_struct = *maybe<in_place_struct>{in_place, 10, 10};

         CHECK(my_struct.x() == 10);
         CHECK(my_struct.y() == 10);
      }
      SUBCASE("non-trivial")
      {
         std::string str = *maybe<std::string>{"hello"};

         CHECK(str == "hello");
      }
   }

   TEST_CASE("has_value")
   {
      SUBCASE("non-mutable empty maybe")
      {
         CHECK(noexcept(maybe<int>{none}) == true);

         const maybe<int> empty_maybe = none;

         CHECK(empty_maybe.has_value() == false);
      }

      SUBCASE("mutable empty maybe")
      {
         CHECK(noexcept(maybe<int>{none}) == true);

         maybe<int> empty_maybe = none;

         CHECK(empty_maybe.has_value() == false);

         empty_maybe = maybe<int>{10};

         CHECK(empty_maybe.has_value() == true);
      }

      SUBCASE("const non-empty maybe")
      {
         using my_tuple = std::tuple<int, std::string, in_place_struct>;

         CHECK(noexcept(maybe<my_tuple>{in_place, 10, "hello", in_place_struct{}}) == false);

         const maybe<my_tuple> m{in_place, 10, "hello", in_place_struct{}};

         REQUIRE(m.has_value() == true);
      }

      SUBCASE("non-empty maybe")
      {
         using my_tuple = std::tuple<int, std::string, in_place_struct>;

         CHECK(noexcept(maybe<my_tuple>{in_place, 10, "hello", in_place_struct{}}) == false);

         maybe<my_tuple> m{in_place, 10, "hello", in_place_struct{}};

         REQUIRE(m.has_value() == true);

         m.reset();

         REQUIRE(m.has_value() == false);
      }
   }

   TEST_CASE("value const l-value")
   {
      SUBCASE("trivial")
      {
         const maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value());
         const int my_value = my_maybe.value();

         CHECK(my_value == 10);
      }
      SUBCASE("non-trivial")
      {
         const maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe.has_value());
         const auto& my_value = my_maybe.value();

         CHECK(my_value == std::string{"hello"});
      }
   }
   TEST_CASE("value l-value")
   {
      SUBCASE("trivial")
      {
         maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value());
         int& my_value = my_maybe.value();

         CHECK(my_value == 10);

         my_value = 20;

         CHECK(my_value == 20);
         CHECK(my_maybe.value() == 20);
      }
      SUBCASE("non-trivial")
      {
         maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe.has_value());
         auto& my_value = my_maybe.value();

         CHECK(my_value == std::string{"hello"});

         my_value.append("!");

         CHECK(my_maybe.value() == std::string{"hello!"});
      }
   }
   TEST_CASE("value const r-value")
   {
      SUBCASE("trivial")
      {
         {
            maybe<int> my_maybe{10};

            REQUIRE(my_maybe.has_value());

            const int my_value = std::move(my_maybe).value(); // NOLINT

            CHECK(my_maybe.has_value()); // move on trivial is a copy, doesn't do anything
            CHECK(my_value == 10);
         }
         {
            const int my_value = maybe<int>{10}.value();

            CHECK(my_value == 10);
         }
      }
      SUBCASE("non-trivial")
      {
         {
            maybe<std::string> my_maybe{"10"};

            REQUIRE(my_maybe.has_value());

            const std::string my_value = std::move(my_maybe).value(); // NOLINT

            CHECK(my_maybe.has_value());
            CHECK(my_maybe.value() == std::string{});
            CHECK(my_value == std::string{"10"});
         }
         {
            auto create_maybe = [](const std::string& str) -> const maybe<std::string> {
               return {str};
            };

            const std::string my_value = create_maybe("hello").value();

            CHECK(my_value == std::string{"hello"});
         }
      }
   }
   TEST_CASE("value r-value")
   {
      SUBCASE("trivial")
      {
         {
            maybe<int> my_maybe{10};

            REQUIRE(my_maybe.has_value());

            const int my_value = std::move(my_maybe).value(); // NOLINT

            CHECK(my_maybe.has_value()); // move on trivial is a copy, doesn't do anything
            CHECK(my_value == 10);
         }
         {
            const int my_value = maybe<int>{10}.value();

            CHECK(my_value == 10);
         }
      }
      SUBCASE("non-trivial")
      {
         {
            maybe<std::string> my_maybe{"10"};

            REQUIRE(my_maybe.has_value());

            const std::string my_value = std::move(my_maybe).value(); // NOLINT

            CHECK(my_maybe.has_value());
            CHECK(my_maybe.value() == std::string{});
            CHECK(my_value == std::string{"10"});
         }
         {
            const std::string my_value = maybe<std::string>{"hello"}.value();

            CHECK(my_value == std::string{"hello"});
         }
      }
   }

   TEST_CASE("take")
   {
      SUBCASE("trivial")
      {
         maybe<int> my_first_maybe{10};

         REQUIRE(my_first_maybe.has_value());
         CHECK(my_first_maybe.value() == 10);

         maybe taken = my_first_maybe.take();

         REQUIRE(taken.has_value());
         CHECK(taken.value() == 10);
         CHECK(!my_first_maybe.has_value());
      }
      SUBCASE("non-trivial")
      {
         maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == std::string{"hello"});

         maybe taken = my_maybe.take();

         REQUIRE(taken.has_value());
         CHECK(taken.value() == std::string{"hello"});
         CHECK(!my_maybe.has_value());
      }
   }
   TEST_CASE("swap")
   {
      SUBCASE("trivial")
      {
         {
            maybe<int> maybe_one{10};
            maybe<int> maybe_two{20};

            REQUIRE(maybe_one.has_value());
            CHECK(maybe_one.value() == 10);

            REQUIRE(maybe_two.has_value());
            CHECK(maybe_two.value() == 20);

            maybe_one.swap(maybe_two);

            REQUIRE(maybe_one.has_value());
            CHECK(maybe_one.value() == 20);

            REQUIRE(maybe_two.has_value());
            CHECK(maybe_two.value() == 10);
         }
         {
            maybe<int> maybe_one{10};
            maybe<int> maybe_two{};

            REQUIRE(maybe_one.has_value());
            CHECK(maybe_one.value() == 10);

            REQUIRE(!maybe_two.has_value());

            maybe_one.swap(maybe_two);

            REQUIRE(!maybe_one.has_value());

            REQUIRE(maybe_two.has_value());
            CHECK(maybe_two.value() == 10);
         }
      }
      SUBCASE("non-trivial")
      {
         {
            maybe<std::string> maybe_one{"hello"};
            maybe<std::string> maybe_two{"world"};

            REQUIRE(maybe_one.has_value());
            CHECK(maybe_one.value() == std::string{"hello"});

            REQUIRE(maybe_two.has_value());
            CHECK(maybe_two.value() == std::string{"world"});

            maybe_one.swap(maybe_two);

            REQUIRE(maybe_one.has_value());
            CHECK(maybe_one.value() == std::string{"world"});

            REQUIRE(maybe_two.has_value());
            CHECK(maybe_two.value() == std::string{"hello"});
         }
         {
            maybe<std::string> maybe_one{"hello"};
            maybe<std::string> maybe_two{};

            REQUIRE(maybe_one.has_value());
            CHECK(maybe_one.value() == std::string{"hello"});

            REQUIRE(!maybe_two.has_value());

            maybe_one.swap(maybe_two);

            REQUIRE(!maybe_one.has_value());

            REQUIRE(maybe_two.has_value());
            CHECK(maybe_two.value() == std::string{"hello"});
         }
      }
   }
   TEST_CASE("reset")
   {
      using my_tuple = std::tuple<int, std::string, in_place_struct>;

      CHECK(noexcept(maybe<my_tuple>{in_place, 10, "hello", in_place_struct{}}) == false);

      maybe<my_tuple> m{in_place, 10, "hello", in_place_struct{}};

      REQUIRE(m.has_value() == true);

      m.reset();

      REQUIRE(m.has_value() == false);
   }

   TEST_CASE("map const l-value")
   {
      SUBCASE("trivial")
      {
         const maybe<int> trivial_maybe{10};

         REQUIRE(trivial_maybe.has_value() == true);
         CHECK(trivial_maybe.value() == 10);

         const maybe new_maybe = trivial_maybe.map([](int i) {
            return std::to_string(i);
         });

         REQUIRE(new_maybe.has_value() == true);
         CHECK(trivial_maybe.has_value() == true);
         CHECK(new_maybe.value() == std::string{"10"});
      }
      SUBCASE("non-trivial")
      {
         const maybe<std::string> non_trivial_maybe{"10"};

         REQUIRE(non_trivial_maybe.has_value() == true);
         CHECK(non_trivial_maybe.value() == std::string{"10"});

         const maybe new_maybe = non_trivial_maybe.map([]([[maybe_unused]] const std::string& str) {
            return 10;
         });

         REQUIRE(new_maybe.has_value() == true);
         CHECK(non_trivial_maybe.has_value() == true);
         CHECK(new_maybe.value() == 10);
      }
   }
   TEST_CASE("map l-value")
   {
      SUBCASE("trivial")
      {
         maybe<int> trivial_maybe{10};

         REQUIRE(trivial_maybe.has_value() == true);
         CHECK(trivial_maybe.value() == 10);

         maybe new_maybe = trivial_maybe.map([](int i) {
            return std::to_string(i);
         });

         REQUIRE(new_maybe.has_value() == true);
         CHECK(trivial_maybe.has_value() == true);
         CHECK(new_maybe.value() == std::string{"10"});
      }

      SUBCASE("non-trivial")
      {
         maybe<std::string> non_trivial_maybe{"10"};

         REQUIRE(non_trivial_maybe.has_value() == true);
         CHECK(non_trivial_maybe.value() == std::string{"10"});

         maybe new_maybe = non_trivial_maybe.map([]([[maybe_unused]] const std::string& str) {
            return 10;
         });

         REQUIRE(new_maybe.has_value() == true);
         CHECK(non_trivial_maybe.has_value() == true);
         CHECK(new_maybe.value() == 10);
      }
   }
   TEST_CASE("map const r-value")
   {
      SUBCASE("trivial")
      {
         maybe new_maybe = maybe<int>{10}.map([](int i) {
            return std::to_string(i);
         });

         REQUIRE(new_maybe.has_value() == true);
         CHECK(new_maybe.value() == std::string{"10"});
      }

      SUBCASE("non-trivial")
      {
         maybe new_maybe =
            maybe<std::string>{"10"}.map([]([[maybe_unused]] const std::string& str) {
               return 10;
            });

         REQUIRE(new_maybe.has_value() == true);
         CHECK(new_maybe.value() == 10);

         new_maybe.value() = 20;

         CHECK(new_maybe.value() == 20);
      }
   }
   TEST_CASE("map r-value")
   {
      SUBCASE("trivial")
      {
         maybe new_maybe = maybe<int>{10}.map([](int i) {
            return i * i;
         });

         REQUIRE(new_maybe.has_value() == true);
         CHECK(new_maybe.value() == 100);
      }

      SUBCASE("non-trivial")
      {
         maybe new_maybe =
            maybe<std::string>{"10"}.map([]([[maybe_unused]] const std::string& str) {
               return 10;
            });

         REQUIRE(new_maybe.has_value() == true);
         CHECK(new_maybe.value() == 10);

         new_maybe.value() = 20;

         CHECK(new_maybe.value() == 20);
      }
   }

   TEST_CASE("map_or const l-value")
   {
      SUBCASE("trivial with value")
      {
         const maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value() == true);
         CHECK(my_maybe.value() == 10);

         const int result = my_maybe.map_or(
            [](int i) {
               return i * i;
            },
            0);

         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         const maybe<int> my_maybe{};

         CHECK(my_maybe.has_value() == false);

         const int result = my_maybe.map_or(
            [](int i) {
               return i * i;
            },
            0);

         CHECK(result == 0);
      }
      SUBCASE("non-trivial with value")
      {
         const maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe.has_value() == true);
         CHECK(my_maybe.value() == "hello");

         const std::string result = my_maybe.map_or(
            [](std::string str) {
               return str + ", world!";
            },
            "goodbye");

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         const maybe<std::string> my_maybe{};

         REQUIRE(!my_maybe.has_value());

         const std::string result = my_maybe.map_or(
            [](std::string str) {
               return str + ", world!";
            },
            "goodbye");

         CHECK(result == "goodbye");
      }
   }
   TEST_CASE("map_or l-value")
   {
      SUBCASE("trivial with value")
      {
         maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value() == true);
         CHECK(my_maybe.value() == 10);

         const int result = my_maybe.map_or(
            [](int i) {
               return i * i;
            },
            0);

         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         maybe<int> my_maybe{};

         CHECK(my_maybe.has_value() == false);

         const int result = my_maybe.map_or(
            [](int i) {
               return i * i;
            },
            0);

         CHECK(result == 0);
      }
      SUBCASE("non-trivial with value")
      {
         maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe.has_value() == true);
         CHECK(my_maybe.value() == "hello");

         const std::string result = my_maybe.map_or(
            [](std::string str) {
               return str + ", world!";
            },
            "goodbye");

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         maybe<std::string> my_maybe{};

         REQUIRE(!my_maybe.has_value());

         const std::string result = my_maybe.map_or(
            [](std::string str) {
               return str + ", world!";
            },
            "goodbye");

         CHECK(result == "goodbye");
      }
   }
   TEST_CASE("map_or const r-value")
   {
      const auto get_int = [](int i) -> const maybe<int> {
         if (i <= 10)
         {
            return i;
         }
         else
         {
            return none;
         }
      };
      const auto get_str = [](const std::string& str) -> const maybe<std::string> {
         if (str == std::string{"hello"})
         {
            return str;
         }
         else
         {
            return none;
         }
      };

      SUBCASE("trivial with value")
      {
         const int result = get_int(10).map_or(
            [](int i) {
               return i * i;
            },
            0);

         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         const int result = get_int(20).map_or(
            [](int i) {
               return i * i;
            },
            0);

         CHECK(result == 0);
      }
      SUBCASE("non-trivial with value")
      {
         const std::string result = get_str("hello").map_or(
            [](const std::string&& str) {
               return str + ", world!";
            },
            std::string{"goodbye"});

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         const std::string result = get_str("world").map_or(
            [](const std::string&& str) {
               return str + ", world!";
            },
            std::string{"goodbye"});

         CHECK(result == "goodbye");
      }
   }
   TEST_CASE("map_or r-value")
   {
      SUBCASE("trivial with value")
      {
         const int result = maybe<int>{10}.map_or(
            [](int i) {
               return i * i;
            },
            0);

         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         const int result = maybe<int>{}.map_or(
            [](int i) {
               return i * i;
            },
            0);

         CHECK(result == 0);
      }
      SUBCASE("non-trivial with value")
      {
         const std::string result = maybe<std::string>{"hello"}.map_or(
            [](std::string&& str) {
               return str + ", world!";
            },
            std::string{"goodbye"});

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         const std::string result = maybe<std::string>{}.map_or(
            [](std::string&& str) {
               return str + ", world!";
            },
            std::string{"goodbye"});

         CHECK(result == "goodbye");
      }
   }

   TEST_CASE("and_then const l-value")
   {
      SUBCASE("trivial with value")
      {
         const maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == 10);

         const maybe result = my_maybe.and_then([](const int i) {
            return i * i;
         });

         REQUIRE(result.has_value());
         CHECK(result.value() == 100);
      }
      SUBCASE("trivial without value")
      {
         const maybe<int> my_maybe{};

         CHECK(!my_maybe.has_value());

         const maybe result = my_maybe.and_then([](const int i) {
            return i * i;
         });

         CHECK(!result.has_value());
      }
      SUBCASE("non-trivial with value")
      {
         const maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == "hello");

         const maybe result = my_maybe.and_then([](const std::string& str) {
            return str + ", world!";
         });

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         const maybe<std::string> my_maybe{};

         CHECK(!my_maybe.has_value());

         const maybe result = my_maybe.and_then([](const std::string& str) {
            return str + ", world!";
         });

         CHECK(!result.has_value());
      }
   }
   TEST_CASE("and_then l-value")
   {
      SUBCASE("trivial with value")
      {
         maybe<int> my_maybe{10};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == 10);

         const maybe result = my_maybe.and_then([](const int i) {
            return i * i;
         });

         REQUIRE(result.has_value());
         CHECK(result.value() == 100);
      }
      SUBCASE("trivial without value")
      {
         maybe<int> my_maybe{};

         CHECK(!my_maybe.has_value());

         const maybe result = my_maybe.and_then([](const int i) {
            return i * i;
         });

         CHECK(!result.has_value());
      }
      SUBCASE("non-trivial with value")
      {
         maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe.has_value());
         CHECK(my_maybe.value() == "hello");

         const maybe result = my_maybe.and_then([](const std::string& str) {
            return str + ", world!";
         });

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         maybe<std::string> my_maybe{};

         CHECK(!my_maybe.has_value());

         const maybe result = my_maybe.and_then([](const std::string& str) {
            return str + ", world!";
         });

         CHECK(!result.has_value());
      }
   }
   TEST_CASE("and_then const r-value")
   {
      const auto get_int = [](int i) -> const maybe<int> {
         if (i <= 10)
         {
            return i;
         }
         else
         {
            return none;
         }
      };
      const auto get_str = [](const std::string& str) -> const maybe<std::string> {
         if (str == std::string{"hello"})
         {
            return str;
         }
         else
         {
            return none;
         }
      };

      SUBCASE("trivial with value")
      {
         const maybe result = get_int(10).and_then([](int i) {
            return i * i;
         });

         REQUIRE(result.has_value());
         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         const maybe result = get_int(20).and_then([](int i) {
            return i * i;
         });

         CHECK(!result);
      }
      SUBCASE("non-trivial with value")
      {
         const maybe result = get_str("hello").and_then([](const std::string&& str) {
            return str + ", world!";
         });

         REQUIRE(result);
         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         const maybe result = get_str("world").and_then([](const std::string&& str) {
            return str + ", world!";
         });

         CHECK(!result);
      }
   }
   TEST_CASE("and_then r-value")
   {
      SUBCASE("trivial with value")
      {
         const maybe result = maybe<int>{10}.and_then([](int i) {
            return i * i;
         });

         REQUIRE(result);
         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         const maybe result = maybe<int>{}.and_then([](int i) {
            return i * i;
         });

         CHECK(!result);
      }
      SUBCASE("non-trivial with value")
      {
         const maybe result = maybe<std::string>{"hello"}.and_then([](std::string&& str) {
            return str + ", world!";
         });

         REQUIRE(result);
         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         const maybe result = maybe<std::string>{}.and_then([](std::string&& str) {
            return str + ", world!";
         });

         CHECK(!result);
      }
   }

   TEST_CASE("or_else const l-value")
   {
      SUBCASE("trivial with value")
      {
         const maybe<int> my_maybe{10};

         REQUIRE(my_maybe);
         CHECK(my_maybe == 10);

         int test = 0;
         const maybe result = my_maybe.or_else([&test] {
            test = 10;
         });

         REQUIRE(result);
         CHECK(result == 10);
         CHECK(test == 0);
      }
      SUBCASE("trivial without value")
      {
         const maybe<int> my_maybe{};

         CHECK(!my_maybe.has_value());

         int test = 0;
         const maybe result = my_maybe.or_else([&test] {
            test = 10;
         });

         CHECK(!result.has_value());
         CHECK(test == 10);
      }
      SUBCASE("non-trivial with value")
      {
         const maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe);
         CHECK(my_maybe == "hello");

         std::string other{};
         const maybe result = my_maybe.or_else([&other] {
            other = "goodbye";
         });

         REQUIRE(result);
         CHECK(result == "hello");
         CHECK(other == std::string{});
      }
      SUBCASE("non-trivial without value")
      {
         const maybe<std::string> my_maybe{};

         CHECK(!my_maybe.has_value());

         std::string other{};
         const maybe result = my_maybe.or_else([&other] {
            other = "goodbye";
         });

         CHECK(!result.has_value());
         CHECK(other == "goodbye");
      }
   }
   TEST_CASE("or_else l-value")
   {
      SUBCASE("trivial with value")
      {
         maybe<int> my_maybe{10};

         REQUIRE(my_maybe);
         CHECK(my_maybe == 10);

         int test = 0;
         const maybe result = my_maybe.or_else([&test] {
            test = 10;
         });

         REQUIRE(result);
         CHECK(result == 10);
         CHECK(test == 0);
      }
      SUBCASE("trivial without value")
      {
         maybe<int> my_maybe{};

         CHECK(!my_maybe.has_value());

         int test = 0;
         const maybe result = my_maybe.or_else([&test] {
            test = 10;
         });

         CHECK(!result.has_value());
         CHECK(test == 10);
      }
      SUBCASE("non-trivial with value")
      {
         maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe);
         CHECK(my_maybe == "hello");

         std::string other{};
         const maybe result = my_maybe.or_else([&other] {
            other = "goodbye";
         });

         REQUIRE(result);
         CHECK(result == "hello");
         CHECK(other == std::string{});
      }
      SUBCASE("non-trivial without value")
      {
         maybe<std::string> my_maybe{};

         CHECK(!my_maybe.has_value());

         std::string other{};
         const maybe result = my_maybe.or_else([&other] {
            other = "goodbye";
         });

         CHECK(!result.has_value());
         CHECK(other == "goodbye");
      }
   }
   TEST_CASE("or_else const r-value")
   {
      const auto get_int = [](int i) -> const maybe<int> {
         if (i <= 10)
         {
            return i;
         }
         else
         {
            return none;
         }
      };
      const auto get_str = [](const std::string& str) -> const maybe<std::string> {
         if (str == std::string{"hello"})
         {
            return str;
         }
         else
         {
            return none;
         }
      };

      SUBCASE("trivial with value")
      {
         int test = 0;
         const maybe result = get_int(10).or_else([&test] {
            test = 10;
         });

         REQUIRE(result);
         CHECK(result == 10);
         CHECK(test == 0);
      }
      SUBCASE("trivial without value")
      {
         int test = 0;
         const maybe result = get_int(20).or_else([&test] {
            test = 10;
         });

         CHECK(!result);
         CHECK(test == 10);
      }
      SUBCASE("non-trivial with value")
      {
         std::string other{};
         const maybe result = get_str("hello").or_else([&other] {
            other = "goodbye";
         });

         REQUIRE(result);
         CHECK(result == "hello");
         CHECK(other == std::string{});
      }
      SUBCASE("non-trivial without value")
      {
         std::string other{};
         const maybe result = get_str("world").or_else([&other] {
            other = "goodbye";
         });

         CHECK(!result.has_value());
         CHECK(other == "goodbye");
      }
   }
   TEST_CASE("or_else r-value")
   {
      SUBCASE("trivial with value")
      {
         int test = 0;
         const maybe result = maybe<int>{10}.or_else([&test] {
            test = 10;
         });

         REQUIRE(result);
         CHECK(result == 10);
         CHECK(test == 0);
      }
      SUBCASE("trivial without value")
      {
         int test = 0;
         const maybe result = maybe<int>{}.or_else([&test] {
            test = 10;
         });

         CHECK(!result);
         CHECK(test == 10);
      }
      SUBCASE("non-trivial with value")
      {
         std::string other{};
         const maybe result = maybe<std::string>{"hello"}.or_else([&other] {
            other = "goodbye";
         });

         REQUIRE(result);
         CHECK(result == "hello");
         CHECK(other == std::string{});
      }
      SUBCASE("non-trivial without value")
      {
         std::string other{};
         const maybe result = maybe<std::string>{}.or_else([&other] {
            other = "goodbye";
         });

         CHECK(!result.has_value());
         CHECK(other == "goodbye");
      }
   }

   TEST_CASE("map_or_else const l-value")
   {
      SUBCASE("trivial with value")
      {
         const maybe<int> my_maybe{10};

         REQUIRE(my_maybe);
         CHECK(my_maybe == 10);

         const int result = my_maybe.map_or_else(
            [](int i) {
               return i * i;
            },
            [] {
               return std::numeric_limits<int>::max();
            });

         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         const maybe<int> my_maybe{};

         CHECK(!my_maybe.has_value());

         const int result = my_maybe.map_or_else(
            [](int i) {
               return i * i;
            },
            [] {
               return std::numeric_limits<int>::max();
            });

         CHECK(result == std::numeric_limits<int>::max());
      }
      SUBCASE("non-trivial with value")
      {
         const maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe);
         CHECK(my_maybe == "hello");

         const std::string result = my_maybe.map_or_else(
            [](const std::string& str) {
               return str + ", world!";
            },
            [] {
               return std::string{"goodbye"};
            });

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         maybe<std::string> my_maybe{};

         CHECK(!my_maybe.has_value());

         const std::string result = my_maybe.map_or_else(
            [](const std::string& str) {
               return str + ", world!";
            },
            [] {
               return std::string{"goodbye"};
            });

         CHECK(result == "goodbye");
      }
   }
   TEST_CASE("map_or_else l-value")
   {
      SUBCASE("trivial with value")
      {
         maybe<int> my_maybe{10};

         REQUIRE(my_maybe);
         CHECK(my_maybe == 10);

         const int result = my_maybe.map_or_else(
            [](int i) {
               return i * i;
            },
            [] {
               return std::numeric_limits<int>::max();
            });

         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         maybe<int> my_maybe{};

         CHECK(!my_maybe.has_value());

         const int result = my_maybe.map_or_else(
            [](int i) {
               return i * i;
            },
            [] {
               return std::numeric_limits<int>::max();
            });

         CHECK(result == std::numeric_limits<int>::max());
      }
      SUBCASE("non-trivial with value")
      {
         maybe<std::string> my_maybe{"hello"};

         REQUIRE(my_maybe);
         CHECK(my_maybe == "hello");

         const auto result = my_maybe.map_or_else(
            [](const std::string& str) {
               return str + ", world!";
            },
            [] {
               return std::string{"goodbye"};
            });

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         maybe<std::string> my_maybe{};

         CHECK(!my_maybe);

         const std::string result = my_maybe.map_or_else(
            [](const std::string& str) -> std::string {
               return str + ", world!";
            },
            [] {
               return std::string{"goodbye"};
            });

         CHECK(result == "goodbye");
      }
   }
   TEST_CASE("map_or_else const r-value")
   {
      const auto get_int = [](int i) -> const maybe<int> {
         if (i <= 10)
         {
            return i;
         }
         else
         {
            return none;
         }
      };
      const auto get_str = [](const std::string& str) -> const maybe<std::string> {
         if (str == std::string{"hello"})
         {
            return str;
         }
         else
         {
            return none;
         }
      };

      SUBCASE("trivial with value")
      {
         const int result = get_int(10).map_or_else(
            [](int i) {
               return i * i;
            },
            [] {
               return std::numeric_limits<int>::max();
            });

         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         const int result = get_int(20).map_or_else(
            [](int i) {
               return i * i;
            },
            [] {
               return std::numeric_limits<int>::max();
            });

         CHECK(result == std::numeric_limits<int>::max());
      }
      SUBCASE("non-trivial with value")
      {
         const std::string result = get_str("hello").map_or_else(
            [](const std::string&& str) {
               return str + ", world!";
            },
            [] {
               return std::string{"goodbye"};
            });

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         const std::string result = get_str("world").map_or_else(
            [](const std::string&& str) {
               return str + ", world";
            },
            [] {
               return std::string{"goodbye"};
            });

         CHECK(result == "goodbye");
      }
   }
   TEST_CASE("map_or_else r-value")
   {
      SUBCASE("trivial with value")
      {
         const int result = maybe<int>{10}.map_or_else(
            [](int i) {
               return i * i;
            },
            [] {
               return std::numeric_limits<int>::max();
            });

         CHECK(result == 100);
      }
      SUBCASE("trivial without value")
      {
         const int result = maybe<int>{}.map_or_else(
            [](int i) {
               return i * i;
            },
            [] {
               return std::numeric_limits<int>::max();
            });

         CHECK(result == std::numeric_limits<int>::max());
      }
      SUBCASE("non-trivial with value")
      {
         const std::string result = maybe<std::string>{"hello"}.map_or_else(
            [](std::string&& str) {
               return str + ", world!";
            },
            [] {
               return std::string{"goodbye"};
            });

         CHECK(result == "hello, world!");
      }
      SUBCASE("non-trivial without value")
      {
         const std::string result = maybe<std::string>{}.map_or_else(
            [](std::string&& str) {
               return str + ", world";
            },
            [] {
               return std::string{"goodbye"};
            });

         CHECK(result == "goodbye");
      }
   }

   TEST_CASE("make_maybe")
   {
      SUBCASE("trivial")
      {
         const maybe my_maybe = make_maybe(10U);

         CHECK(noexcept(make_maybe(10U)) == false);

         REQUIRE(my_maybe.has_value() == true);
         CHECK(my_maybe.value() == 10U);
      }
      SUBCASE("non-trivial")
      {
         const maybe my_maybe = make_maybe(std::string{"hello"});

         REQUIRE(my_maybe.has_value() == true);
         CHECK(my_maybe.value() == std::string{"hello"});
      }
   }

   TEST_CASE("maybe to maybe equality")
   {
      SUBCASE("trivial")
      {
         {
            maybe<int> maybe_1{10};
            maybe<int> maybe_2{10};

            CHECK((maybe_1 == maybe_2) == true);
         }
         {
            maybe<int> maybe_1{10};
            maybe<int> maybe_2{20};

            CHECK((maybe_1 == maybe_2) == false);
         }
         {
            maybe<int> maybe_1{10};
            maybe<int> maybe_2{};

            CHECK((maybe_1 == maybe_2) == false);
         }
      }
      SUBCASE("trivial")
      {
         {
            maybe<std::string> maybe_1{"hello"};
            maybe<std::string> maybe_2{"hello"};

            CHECK((maybe_1 == maybe_2) == true);
         }
         {
            maybe<std::string> maybe_1{"hello"};
            maybe<std::string> maybe_2{"hello!"};

            CHECK((maybe_1 == maybe_2) == false);
         }
         {
            maybe<std::string> maybe_1{"hello"};
            maybe<std::string> maybe_2{};

            CHECK((maybe_1 == maybe_2) == false);
         }
      }
   }
   TEST_CASE("maybe to none equality")
   {
      SUBCASE("trivial")
      {
         maybe<int> my_maybe{10};
         maybe<int> empty{};

         CHECK((my_maybe == none) == false);
         CHECK((my_maybe != none) == true);
         CHECK((empty == none) == true);
      }
      SUBCASE("non-trivial")
      {
         maybe<std::string> my_maybe{"hello"};
         maybe<std::string> empty{};

         CHECK((my_maybe == none) == false);
         CHECK((my_maybe != none) == true);
         CHECK((empty == none) == true);
      }
   }
   TEST_CASE("maybe to any equality")
   {
      SUBCASE("trivial")
      {
         maybe<int> my_maybe{10};

         CHECK((my_maybe == 10) == true);
         CHECK((my_maybe != 100) == true);
      }
      SUBCASE("non-trivial")
      {
         maybe<std::string> my_maybe{"hello"};

         CHECK((my_maybe == std::string{"hello"}) == true);
         CHECK((my_maybe != std::string{"hello!"}) == true);
      }
   }

   TEST_CASE("maybe to maybe comparison")
   {
      SUBCASE("trivial >=")
      {
         {
            maybe<int> first_maybe{10};
            maybe<int> second_maybe{10};

            CHECK((first_maybe >= second_maybe) == true);
            CHECK((second_maybe >= first_maybe) == true);
         }
         {
            maybe<int> first_maybe{10};
            maybe<int> second_maybe{11};

            CHECK((first_maybe >= second_maybe) == false);
            CHECK((second_maybe >= first_maybe) == true);
         }
      }
      SUBCASE("trivial >")
      {
         {
            maybe<int> first_maybe{10};
            maybe<int> second_maybe{10};

            CHECK((first_maybe > second_maybe) == false);
            CHECK((second_maybe > first_maybe) == false);
         }
         {
            maybe<int> first_maybe{10};
            maybe<int> second_maybe{11};

            CHECK((first_maybe > second_maybe) == false);
            CHECK((second_maybe > first_maybe) == true);
         }
      }
      SUBCASE("trivial <=")
      {
         {
            maybe<int> first_maybe{10};
            maybe<int> second_maybe{10};

            CHECK((first_maybe <= second_maybe) == true);
            CHECK((second_maybe <= first_maybe) == true);
         }
         {
            maybe<int> first_maybe{10};
            maybe<int> second_maybe{11};

            CHECK((first_maybe <= second_maybe) == true);
            CHECK((second_maybe <= first_maybe) == false);
         }
      }
      SUBCASE("trivial <")
      {
         {
            maybe<int> first_maybe{10};
            maybe<int> second_maybe{10};

            CHECK((first_maybe < second_maybe) == false);
            CHECK((second_maybe < first_maybe) == false);
         }
         {
            maybe<int> first_maybe{10};
            maybe<int> second_maybe{11};

            CHECK((first_maybe < second_maybe) == true);
            CHECK((second_maybe < first_maybe) == false);
         }
      }
      SUBCASE("non-trivial >=")
      {
         {
            maybe<std::string> first_maybe{"hello"};
            maybe<std::string> second_maybe{"hello"};

            CHECK((first_maybe >= second_maybe) == true);
            CHECK((second_maybe >= first_maybe) == true);
         }
         {
            maybe<std::string> first_maybe{"hello"};
            maybe<std::string> second_maybe{"hell"};
            maybe<std::string> third_maybe{"hello!"};

            CHECK((first_maybe >= second_maybe) == true);
            CHECK((first_maybe >= third_maybe) == false);
            CHECK((second_maybe >= first_maybe) == false);
            CHECK((second_maybe >= third_maybe) == false);
            CHECK((third_maybe >= first_maybe) == true);
            CHECK((third_maybe >= second_maybe) == true);
         }
      }
      SUBCASE("non-trivial >")
      {
         {
            maybe<std::string> first_maybe{"hello"};
            maybe<std::string> second_maybe{"hello"};

            CHECK((first_maybe > second_maybe) == false);
            CHECK((second_maybe > first_maybe) == false);
         }
         {
            maybe<std::string> first_maybe{"hello"};
            maybe<std::string> second_maybe{"hell"};
            maybe<std::string> third_maybe{"hello!"};

            CHECK((first_maybe > second_maybe) == true);
            CHECK((first_maybe > third_maybe) == false);
            CHECK((second_maybe > first_maybe) == false);
            CHECK((second_maybe > third_maybe) == false);
            CHECK((third_maybe > first_maybe) == true);
            CHECK((third_maybe > second_maybe) == true);
         }
      }
      SUBCASE("non-trivial <=")
      {
         {
            maybe<std::string> first_maybe{"hello"};
            maybe<std::string> second_maybe{"hello"};

            CHECK((first_maybe <= second_maybe) == true);
            CHECK((second_maybe <= first_maybe) == true);
         }
         {
            maybe<std::string> first_maybe{"hello"};
            maybe<std::string> second_maybe{"hell"};
            maybe<std::string> third_maybe{"hello!"};

            CHECK((first_maybe <= second_maybe) == false);
            CHECK((first_maybe <= third_maybe) == true);
            CHECK((second_maybe <= first_maybe) == true);
            CHECK((second_maybe <= third_maybe) == true);
            CHECK((third_maybe <= first_maybe) == false);
            CHECK((third_maybe <= second_maybe) == false);
         }
      }
      SUBCASE("non-trivial <")
      {
         {
            maybe<std::string> first_maybe{"hello"};
            maybe<std::string> second_maybe{"hello"};

            CHECK((first_maybe < second_maybe) == false);
            CHECK((second_maybe < first_maybe) == false);
         }
         {
            maybe<std::string> first_maybe{"hello"};
            maybe<std::string> second_maybe{"hell"};
            maybe<std::string> third_maybe{"hello!"};

            CHECK((first_maybe < second_maybe) == false);
            CHECK((first_maybe < third_maybe) == true);
            CHECK((second_maybe < first_maybe) == true);
            CHECK((second_maybe < third_maybe) == true);
            CHECK((third_maybe < first_maybe) == false);
            CHECK((third_maybe < second_maybe) == false);
         }
      }
   }
   TEST_CASE("maybe to none comparison")
   {
      SUBCASE(">=")
      {
         maybe<int> my_maybe{10};

         CHECK((my_maybe >= none) == true);
      }
      SUBCASE(">")
      {
         maybe<int> my_maybe{10};

         CHECK((my_maybe > none) == true);
      }
      SUBCASE("<=")
      {
         maybe<int> my_maybe{10};

         CHECK((my_maybe <= none) == false);
      }
      SUBCASE("<")
      {
         maybe<int> my_maybe{10};

         CHECK((my_maybe < none) == false);
      }
   }
   TEST_CASE("maybe to any comparison")
   {
      SUBCASE("trivial >=")
      {
         maybe<int> my_maybe{10};

         CHECK(noexcept(my_maybe >= 10) == true);

         CHECK((my_maybe >= 10) == true);  // NOLINT
         CHECK((my_maybe >= 0) == true);   // NOLINT
         CHECK((my_maybe >= 20) == false); // NOLINT
      }
      SUBCASE("trivial >")
      {
         maybe<int> my_maybe{10};

         CHECK(noexcept(my_maybe > 10) == true);

         CHECK((my_maybe > 10) == false); // NOLINT
         CHECK((my_maybe > 0) == true);   // NOLINT
         CHECK((my_maybe > 20) == false); // NOLINT
      }
      SUBCASE("trivial <=")
      {
         maybe<int> my_maybe{10};

         CHECK(noexcept(my_maybe <= 10) == true);

         CHECK((my_maybe <= 10) == true); // NOLINT
         CHECK((my_maybe <= 0) == false); // NOLINT
         CHECK((my_maybe <= 20) == true); // NOLINT
      }
      SUBCASE("trivial <")
      {
         maybe<int> my_maybe{10};

         CHECK(noexcept(my_maybe < 10) == true);

         CHECK((my_maybe < 10) == false); // NOLINT
         CHECK((my_maybe < 0) == false);  // NOLINT
         CHECK((my_maybe < 20) == true);  // NOLINT
      }
      SUBCASE("non-trivial >=")
      {
         maybe<std::string> my_maybe{"hello"};

         CHECK(noexcept(my_maybe >= std::string{"hello"}) == false);

         CHECK((my_maybe >= std::string{"hello"}) == true);   // NOLINT
         CHECK((my_maybe >= std::string{"hell"}) == true);    // NOLINT
         CHECK((my_maybe >= std::string{"hello!"}) == false); // NOLINT
      }
      SUBCASE("non-trivial >")
      {
         maybe<std::string> my_maybe{"hello"};

         CHECK(noexcept(my_maybe > std::string{"hello"}) == false);

         CHECK((my_maybe > std::string{"hello"}) == false);  // NOLINT
         CHECK((my_maybe > std::string{"hell"}) == true);    // NOLINT
         CHECK((my_maybe > std::string{"hello!"}) == false); // NOLINT
      }
      SUBCASE("non-trivial <=")
      {
         maybe<std::string> my_maybe{"hello"};

         CHECK(noexcept(my_maybe <= std::string{"hello"}) == false);

         CHECK((my_maybe <= std::string{"hello"}) == true);  // NOLINT
         CHECK((my_maybe <= std::string{"hell"}) == false);  // NOLINT
         CHECK((my_maybe <= std::string{"hello!"}) == true); // NOLINT
      }
      SUBCASE("non-trivial <")
      {
         maybe<std::string> my_maybe{"hello"};

         CHECK(noexcept(my_maybe < std::string{"hello"}) == false);

         CHECK((my_maybe < std::string{"hello"}) == false); // NOLINT
         CHECK((my_maybe < std::string{"hell"}) == false);  // NOLINT
         CHECK((my_maybe < std::string{"hello!"}) == true); // NOLINT
      }
   }

   TEST_CASE("std::swap")
   {
      SUBCASE("both valid")
      {
         maybe<int> first_maybe{10};
         maybe<int> second_maybe{20};

         REQUIRE(first_maybe.has_value() == true);
         CHECK(first_maybe.value() == 10);
         REQUIRE(second_maybe.has_value() == true);
         CHECK(second_maybe.value() == 20);

         CHECK(noexcept(std::swap(first_maybe, second_maybe)) == true);

         std::swap(first_maybe, second_maybe);

         REQUIRE(first_maybe.has_value() == true);
         CHECK(first_maybe.value() == 20);
         REQUIRE(second_maybe.has_value() == true);
         CHECK(second_maybe.value() == 10);
      }

      SUBCASE("one valid")
      {
         {
            maybe<int> first_maybe{10};
            maybe<int> second_maybe{none};

            REQUIRE(first_maybe.has_value() == true);
            CHECK(first_maybe.value() == 10);
            REQUIRE(second_maybe.has_value() == false);

            std::swap(first_maybe, second_maybe);

            CHECK(first_maybe.has_value() == false);
            REQUIRE(second_maybe.has_value() == true);
            CHECK(second_maybe.value() == 10);

            std::swap(first_maybe, second_maybe);

            REQUIRE(first_maybe.has_value() == true);
            CHECK(first_maybe.value() == 10);
            REQUIRE(second_maybe.has_value() == false);
         }
         {
            maybe<std::string> first_maybe{"10"};
            maybe<std::string> second_maybe{none};

            REQUIRE(first_maybe.has_value() == true);
            CHECK(first_maybe.value() == std::string{"10"});
            REQUIRE(second_maybe.has_value() == false);

            std::swap(first_maybe, second_maybe);

            CHECK(first_maybe.has_value() == false);
            REQUIRE(second_maybe.has_value() == true);
            CHECK(second_maybe.value() == std::string{"10"});

            CHECK(noexcept(std::swap(first_maybe, second_maybe)) == true);

            std::swap(first_maybe, second_maybe);

            REQUIRE(first_maybe.has_value() == true);
            CHECK(first_maybe.value() == std::string{"10"});
            REQUIRE(second_maybe.has_value() == false);
         }
      }

      SUBCASE("both invalid")
      {
         maybe<int> first_maybe{none};
         maybe<int> second_maybe{none};

         CHECK(first_maybe.has_value() == false);
         CHECK(second_maybe.has_value() == false);

         std::swap(first_maybe, second_maybe);

         CHECK(first_maybe.has_value() == false);
         CHECK(second_maybe.has_value() == false);
      }
   }
}

TEST_SUITE("result test suite")
{
   TEST_CASE("ctor value copy") {}
   TEST_CASE("ctor value move") {}
   TEST_CASE("ctor value in_place") {}

   TEST_CASE("ctor error copy") {}
   TEST_CASE("ctor error move") {}
   TEST_CASE("ctor error in_place") {}
}

TEST_CASE("result test suite")
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
         monad::error_t<int> i = make_error(0);

         result<int, int> test{i};

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == i.val);
      }
      {
         const auto hello = make_error<std::string>("hello, world!");

         result<int, std::string> test{hello};

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == hello.val);
      }
      {
         const auto hello = make_error<std::string>("hello, world!");
         const auto lambda = [&](int i) noexcept -> result<int, std::string> {
            if (i > 10)
            {
               return hello;
            }
            else
            {
               return i;
            }
         };

         const auto test = lambda(11);

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == hello.val);
      }
   }
   SUBCASE("error move ctor")
   {
      {
         result<int, int> test{make_error<int>(0)};

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == 0);
      }
      {
         result<std::string, std::string> test{make_error<std::string>("hello, world!")};

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == "hello, world!");
      }
      {
         const auto lambda = [&]() noexcept -> result<int, std::string> {
            return make_error<std::string>("hello, world!");
         };

         const auto test = lambda();

         REQUIRE(test.is_value() == false);
         CHECK(test.error().value() == "hello, world!");
      }
      {
         result<std::unique_ptr<int>, int> test{std::make_unique<int>(10)}; // NOLINT

         REQUIRE(test.is_value() == true);
         CHECK(*std::move(test).value().value() == 10);
      }
   }
   SUBCASE("error in-place ctor")
   {
      {
         result<int, int_field> test{error_in_place, 10, 10, 10, 10}; // NOLINT

         REQUIRE(test.is_value() == false);

         const auto field = test.error();

         CHECK(field->m_a == 10);
         CHECK(field->m_b == 10);
         CHECK(field->m_c == 10);
         CHECK(field->m_d == 10);
      }
      {
         result<int, movable_int_field> test{error_in_place, 10, 10, 10, 10}; // NOLINT

         REQUIRE(test.is_value() == false);

         const auto field = std::move(test).error();

         CHECK(field->m_a == 10);
         CHECK(field->m_b == 10);
         CHECK(field->m_c == 10);
         CHECK(field->m_d == 10);
      }
   }

   SUBCASE("const l-value value") {}
   SUBCASE("l-value value") {}
   SUBCASE("const r-value value") {}
   SUBCASE("r-value value") {}

   SUBCASE("const l-value error") {}
   SUBCASE("l-value error") {}
   SUBCASE("const r-value error") {}
   SUBCASE("r-value error") {}

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

   SUBCASE("const l-value map_error") {}
   SUBCASE("l-value map_error") {}
   SUBCASE("const r-value map_error") {}
   SUBCASE("r-value map_error") {}
}

TEST_SUITE("either test suite") {}

TEST_SUITE("try_wrap test suite")
{
   TEST_CASE("maybe return")
   {
      SUBCASE("expection thrown")
      {
         const maybe my_maybe = try_wrap<std::exception>([] {
            throw std::exception{};
         });

         REQUIRE(my_maybe);
      }
      SUBCASE("no exception thrown")
      {
         int value = 0;
         const maybe my_maybe = try_wrap<std::exception>([&] {
            value = 10;
         });

         REQUIRE(!my_maybe);
         CHECK(value == 10);
      }
   }
   TEST_CASE("result return")
   {
      SUBCASE("exception thrown")
      {
         const result my_result = try_wrap<std::exception>([] {
            throw std::exception{};
            return 0;
         });

         REQUIRE(!my_result);
      }
      SUBCASE("normal value returned")
      {
         const result my_result = try_wrap<std::exception>([] {
            return 0;
         });

         REQUIRE(my_result);
         CHECK(my_result.value() == 0);
      }
   }
}
