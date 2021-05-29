#include <libreglisse/maybe.hpp>

#include <doctest/doctest.h>

using namespace reglisse;

TEST_SUITE("reglisse::maybe - maybe test suite") // NOLINT
{
   TEST_CASE("maybe()")
   {
      maybe<int> maybe_int{};
      maybe<float> maybe_float{};
      maybe<std::vector<int>> maybe_vector{};

      CHECK(maybe_int.is_none());
      CHECK(maybe_float.is_none());
      CHECK(maybe_vector.is_none());
   }
   TEST_CASE("maybe(none_t)")
   {
      maybe<int> maybe_int{none};

      CHECK(maybe_int.is_none());
   }
   TEST_CASE("maybe(const some<T>&)")
   {
      const auto some_int = some<int>(1);
      const auto some_float = some<float>(1.0f);
      const auto some_vector = some<std::vector<int>>({1, 1, 1});

      maybe<int> maybe_int{some_int};
      maybe<float> maybe_float{some_float};
      maybe<std::vector<int>> maybe_vector{some_vector};

      CHECK(maybe_int.is_some());
      CHECK(maybe_int.value() == some_int.value());
      CHECK(maybe_float.is_some());
      CHECK(maybe_float.value() == some_float.value());
      CHECK(maybe_vector.is_some());
      CHECK(maybe_vector.value() == some_vector.value());
   }
   TEST_CASE("maybe(some<T>&&)")
   {
      maybe<int> maybe_int{some<int>(1)};
      maybe<float> maybe_float{some<float>(1.0f)};
      maybe<std::vector<int>> maybe_vector{some<std::vector<int>>({1, 1, 1})};

      CHECK(maybe_int.is_some());
      CHECK(maybe_int.value() == 1);
      CHECK(maybe_float.is_some());
      CHECK(maybe_float.value() == 1.0f);
      CHECK(maybe_vector.is_some());
      CHECK(maybe_vector.value() == std::vector<int>({1, 1, 1}));
   }
   TEST_CASE("maybe(const maybe&)")
   {
      const maybe<int> maybe_int{some<int>(1)};
      const maybe<float> maybe_float{some<float>(1.0f)};
      const maybe<std::vector<int>> maybe_vector{some<std::vector<int>>({1, 1, 1})};

      REQUIRE(maybe_int.is_some());
      REQUIRE(maybe_float.is_some());
      REQUIRE(maybe_vector.is_some());

      maybe<int> maybe_int_2{maybe_int};                    // NOLINT
      maybe<float> maybe_float_2{maybe_float};              // NOLINT
      maybe<std::vector<int>> maybe_vector_2{maybe_vector}; // NOLINT

      REQUIRE(maybe_int_2.is_some());
      REQUIRE(maybe_float_2.is_some());
      REQUIRE(maybe_vector_2.is_some());

      CHECK(maybe_int.value() == maybe_int_2.value());
      CHECK(maybe_float.value() == maybe_float_2.value());
      CHECK(maybe_vector.value() == maybe_vector_2.value());
   }
   TEST_CASE("maybe(maybe&&)")
   {
      maybe<int> maybe_int{some<int>(1)};
      maybe<float> maybe_float{some<float>(1.0f)};
      maybe<std::vector<int>> maybe_vector{some<std::vector<int>>({1, 1, 1})};

      REQUIRE(maybe_int.is_some());
      REQUIRE(maybe_float.is_some());
      REQUIRE(maybe_vector.is_some());

      maybe<int> maybe_int_2{std::move(maybe_int)};
      maybe<float> maybe_float_2{std::move(maybe_float)};
      maybe<std::vector<int>> maybe_vector_2{std::move(maybe_vector)};

      REQUIRE(maybe_int_2.is_some());
      REQUIRE(maybe_float_2.is_some());
      REQUIRE(maybe_vector_2.is_some());

      CHECK(maybe_int_2.value() == 1);
      CHECK(maybe_float_2.value() == 1.0f);
      CHECK(maybe_vector_2.value() == std::vector<int>({1, 1, 1}));
   }

   TEST_CASE("operator=(const maybe&)")
   {
      const maybe<int> maybe_int{some<int>(1)};
      const maybe<float> maybe_float{some<float>(1.0f)};
      const maybe<std::vector<int>> maybe_vector{some<std::vector<int>>({1, 1, 1})};

      REQUIRE(maybe_int.is_some());
      REQUIRE(maybe_float.is_some());
      REQUIRE(maybe_vector.is_some());

      maybe<int> maybe_int_2 = maybe_int;                    // NOLINT
      maybe<float> maybe_float_2 = maybe_float;              // NOLINT
      maybe<std::vector<int>> maybe_vector_2 = maybe_vector; // NOLINT

      REQUIRE(maybe_int_2.is_some());
      REQUIRE(maybe_float_2.is_some());
      REQUIRE(maybe_vector_2.is_some());

      CHECK(maybe_int.value() == maybe_int_2.value());
      CHECK(maybe_float.value() == maybe_float_2.value());
      CHECK(maybe_vector.value() == maybe_vector_2.value());
   }
   TEST_CASE("operator=(maybe&&)")
   {
      maybe<int> maybe_int{some<int>(1)};
      maybe<float> maybe_float{some<float>(1.0f)};
      maybe<std::vector<int>> maybe_vector{some<std::vector<int>>({1, 1, 1})};

      REQUIRE(maybe_int.is_some());
      REQUIRE(maybe_float.is_some());
      REQUIRE(maybe_vector.is_some());

      maybe<int> maybe_int_2 = std::move(maybe_int);                    // NOLINT
      maybe<float> maybe_float_2 = std::move(maybe_float);              // NOLINT
      maybe<std::vector<int>> maybe_vector_2 = std::move(maybe_vector); // NOLINT

      REQUIRE(maybe_int_2.is_some());
      REQUIRE(maybe_float_2.is_some());
      REQUIRE(maybe_vector_2.is_some());

      CHECK(maybe_int_2.value() == 1);
      CHECK(maybe_float_2.value() == 1.0f);
      CHECK(maybe_vector_2.value() == std::vector<int>({1, 1, 1}));

      maybe maybe_float_vec = some(std::vector<float>{1.0f, 1.0f});

      REQUIRE(maybe_float_vec.is_some());
      CHECK(maybe_float_vec.value() == std::vector<float>({1.0f, 1.0f}));

      maybe<int> maybe_none = none;

      CHECK(maybe_none.is_none());
   }

   TEST_CASE("value() &")
   {
      maybe maybe_int = some(1);
      maybe maybe_vec = some(std::vector({1, 1}));

      CHECK(maybe_int.value() == 1);
      CHECK(maybe_vec.value() == std::vector({1, 1}));
   }
   TEST_CASE("value() const&")
   {
      const maybe maybe_int = some(1);
      const maybe maybe_vec = some(std::vector({1, 1}));

      CHECK(maybe_int.value() == 1);
      CHECK(maybe_vec.value() == std::vector({1, 1}));
   }
   TEST_CASE("take() const&&")
   {
      const auto maybe_int_l = []() -> const maybe<int> {
         return some(1);
      };

      const auto val = maybe_int_l().take();

      CHECK(val == 1);
   }
   TEST_CASE("take() &&")
   {
      maybe maybe_int = some(1);
      maybe maybe_vec = some(std::vector({1, 1}));

      CHECK(std::move(maybe_int).take() == 1);
      CHECK(std::move(maybe_vec).take() == std::vector({1, 1}));
   }

   TEST_CASE("take_or(U&&) const&&")
   {
      const auto maybe_int_l = [](bool b) -> const maybe<int> {
         if (b)
         {
            return some(1);
         }

         return none;
      };

      CHECK(maybe_int_l(true).take_or(0) == 1);
      CHECK(maybe_int_l(false).take_or(0) == 0);
   }
   TEST_CASE("take_or(U&&) &&")
   {
      maybe maybe_int = some(1);
      maybe maybe_vec = some(std::vector({1, 1}));

      maybe<std::string> maybe_str = none;

      CHECK(std::move(maybe_int).take_or(0) == 1);
      CHECK(std::move(maybe_vec).take_or(std::vector<int>{}) == std::vector({1, 1}));

      CHECK(maybe<float>(none).take_or(0.0f) == 0.0f);
      CHECK(maybe<std::string>(none).take_or("hello") == "hello");
   }

   TEST_CASE("reset()")
   {
      maybe maybe_int = some(1);
      maybe maybe_float = some(1.0f);
      maybe maybe_vector = some(std::vector({1, 1, 1}));

      REQUIRE(maybe_int.is_some());
      CHECK(maybe_int == 1);
      REQUIRE(maybe_float.is_some());
      CHECK(maybe_float == 1.0f);
      REQUIRE(maybe_vector.is_some());
      CHECK(maybe_vector == std::vector({1, 1, 1}));

      maybe_int.reset();
      maybe_float.reset();
      maybe_vector.reset();

      REQUIRE(maybe_int.is_none());
      REQUIRE(maybe_float.is_none());
      REQUIRE(maybe_vector.is_none());
   }

   TEST_CASE("swap(maybe&)")
   {
      maybe int_zero = some(0);
      maybe int_one = some(1);

      CHECK(int_zero == 0);
      CHECK(int_one == 1);

      int_zero.swap(int_one);

      CHECK(int_zero == 1);
      CHECK(int_one == 0);

      maybe str_hello = some(std::string("hello"));
      maybe str_bye = some(std::string("bye"));

      CHECK(str_hello == "hello");
      CHECK(str_bye == "bye");

      str_hello.swap(str_bye);

      CHECK(str_hello == "bye");
      CHECK(str_bye == "hello");
   }

   TEST_CASE("is_some()")
   {
      maybe maybe_int = some(1);
      maybe<float> maybe_float = none;
      maybe maybe_vec = some(std::vector({1, 1}));
      maybe<std::string> maybe_str{};

      CHECK(maybe_int.is_some());
      CHECK(not maybe_float.is_some());
      CHECK(maybe_vec.is_some());
      CHECK(not maybe_str.is_some());
   }
   TEST_CASE("is_none()")
   {
      maybe maybe_int = some(1);
      maybe<float> maybe_float = none;
      maybe maybe_vec = some(std::vector({1, 1}));
      maybe<std::string> maybe_str{};

      CHECK(not maybe_int.is_none());
      CHECK(maybe_float.is_none());
      CHECK(not maybe_vec.is_none());
      CHECK(maybe_str.is_none());
   }
   TEST_CASE("operator bool()")
   {
      maybe maybe_int = some(1);
      maybe<float> maybe_float = none;
      maybe maybe_vec = some(std::vector({1, 1}));
      maybe<std::string> maybe_str{};

      bool maybe_int_b = maybe_int;
      bool maybe_float_b = maybe_float;
      bool maybe_vec_b = maybe_vec;
      bool maybe_str_b = maybe_str;

      CHECK(maybe_int_b);
      CHECK(not maybe_float_b);
      CHECK(maybe_vec_b);
      CHECK(not maybe_str_b);
   }

   TEST_CASE("transform(fun&&) const&")
   {
      const maybe maybe_int = some(1);
      const maybe maybe_float = maybe_int.transform([](int val) {
         return static_cast<float>(val);
      });
      const maybe maybe_vec = maybe_float.transform([](float val) {
         return std::vector<float>({val, val});
      });

      const maybe<int> maybe_none = none;
      const maybe<std::string> maybe_str = maybe_none.transform([](int) {
         return std::string("some_val");
      });

      REQUIRE(maybe_int.is_some());
      CHECK(maybe_int.value() == 1);
      REQUIRE(maybe_float.is_some());
      CHECK(maybe_float.value() == 1.0f);
      REQUIRE(maybe_vec.is_some());
      CHECK(maybe_vec.value() == std::vector<float>({1.0f, 1.0f}));
      REQUIRE(maybe_none.is_none());
      REQUIRE(maybe_str.is_none());
   }
   TEST_CASE("transform(fun&&) &")
   {
      maybe maybe_int = some(1);
      maybe maybe_float = maybe_int.transform([](int val) {
         return static_cast<float>(val);
      });
      maybe maybe_vec = maybe_float.transform([](float val) {
         return std::vector<float>({val, val});
      });

      maybe<int> maybe_none = none;
      maybe maybe_str = maybe_none.transform([](int) {
         return std::string("some_val");
      });

      REQUIRE(maybe_int.is_some());
      CHECK(maybe_int.value() == 1);
      REQUIRE(maybe_float.is_some());
      CHECK(maybe_float.value() == 1.0f);
      REQUIRE(maybe_vec.is_some());
      CHECK(maybe_vec.value() == std::vector<float>({1.0f, 1.0f}));
      REQUIRE(maybe_none.is_none());
      REQUIRE(maybe_str.is_none());
   }
   TEST_CASE("transform(fun&&) const&&")
   {
      const auto maybe_int_l = []() -> const maybe<int> {
         return some(1);
      };

      maybe maybe_int = maybe_int_l().transform([](auto val) {
         return static_cast<float>(val);
      });

      REQUIRE(maybe_int.is_some());
      CHECK(maybe_int.value() == 1.0f);
   }
   TEST_CASE("transform(fun&&) &&")
   {
      const maybe maybe_float = maybe(some(1)).transform([](int val) {
         return float(val);
      });

      REQUIRE(maybe_float.is_some());
      CHECK(maybe_float.value() == 1.0f);

      const maybe maybe_str = maybe(some(1)).transform([](int val) {
         return std::to_string(val);
      });

      REQUIRE(maybe_str.is_some());
      CHECK(maybe_str.value() == "1");

      const maybe<std::vector<int>> maybe_none = maybe<int>(none).transform([](int val) {
         return std::vector<int>({val, val});
      });

      CHECK(maybe_none.is_none());
   }

   TEST_CASE("transform_or(fun&&) const&")
   {
      const maybe maybe_int = some(1);
      const maybe maybe_float = maybe_int.transform([](int val) {
         return static_cast<float>(val);
      });
      const std::vector vec_res = maybe_float.transform_or(
         [](float val) {
            return std::vector<float>({val, val});
         },
         std::vector<float>());

      const maybe<int> maybe_none = none;
      const std::string str_res = maybe_none.transform_or(
         [](int) {
            return std::string("success");
         },
         std::string("failed"));

      REQUIRE(maybe_int.is_some());
      CHECK(maybe_int.value() == 1);
      REQUIRE(maybe_float.is_some());
      CHECK(maybe_float.value() == 1.0f);
      CHECK(vec_res == std::vector<float>({1.0f, 1.0f}));
      REQUIRE(maybe_none.is_none());
      REQUIRE(str_res == "failed");
   }
   TEST_CASE("transform_or(fun&&) &")
   {
      maybe maybe_int = some(1);
      maybe maybe_float = maybe_int.transform([](int val) {
         return static_cast<float>(val);
      });
      const std::vector vec_res = maybe_float.transform_or(
         [](float val) {
            return std::vector<float>({val, val});
         },
         std::vector<float>());

      maybe<int> maybe_none = none;
      const std::string str_res = maybe_none.transform_or(
         [](int) {
            return std::string("success");
         },
         std::string("failed"));

      REQUIRE(maybe_int.is_some());
      CHECK(maybe_int.value() == 1);
      REQUIRE(maybe_float.is_some());
      CHECK(maybe_float.value() == 1.0f);
      CHECK(vec_res == std::vector<float>({1.0f, 1.0f}));
      REQUIRE(maybe_none.is_none());
      REQUIRE(str_res == "failed");
   }
   TEST_CASE("transform_or(fun&&) const&&")
   {
      const auto maybe_int_l = []() -> const maybe<int> {
         return some(1);
      };
      const auto maybe_float_l = []() -> const maybe<float> {
         return none;
      };

      float float_res = maybe_int_l().transform_or(
         [](auto val) {
            return static_cast<float>(val);
         },
         0.0f);

      std::vector vec_res = maybe_float_l().transform_or(
         [](float val) {
            return std::vector({val, val});
         },
         std::vector<float>());

      CHECK(float_res == 1.0f);
      CHECK(vec_res == std::vector<float>());
   }
   TEST_CASE("transform_or(fun&&) &&")
   {
      const float float_res = maybe(some(1)).transform_or(
         [](int val) {
            return float(val);
         },
         0.0f);

      CHECK(float_res == 1.0f);

      std::string str_res = maybe<int>(none).transform_or(
         [](int val) {
            return std::to_string(val);
         },
         std::string());

      CHECK(str_res == std::string());
   }

   TEST_CASE("and_then(fun&&) const&")
   {
      SUBCASE("some")
      {
         const maybe maybe_int = some(1);
         const maybe maybe_float = maybe_int.and_then([](int val) -> maybe<float> {
            if (val == 1)
            {
               return some(static_cast<float>(val));
            }

            return none;
         });

         REQUIRE(maybe_int.is_some());
         CHECK(maybe_int == 1);

         REQUIRE(maybe_float.is_some());
         CHECK(maybe_float == 1.0f);
      }
      SUBCASE("become none")
      {
         const maybe maybe_int = some(1);
         const maybe maybe_float = maybe_int.and_then([](int val) -> maybe<float> {
            if (val != 1)
            {
               return some(static_cast<float>(val));
            }

            return none;
         });

         REQUIRE(maybe_int.is_some());
         CHECK(maybe_int == 1);

         CHECK(maybe_float.is_none());
      }
      SUBCASE("none")
      {
         const maybe<int> maybe_none = none;
         const maybe str_res = maybe_none.and_then([](int val) -> maybe<std::string> {
            return some(std::to_string(val));
         });

         CHECK(maybe_none.is_none());
         CHECK(str_res.is_none());
      }
   }
   TEST_CASE("and_then(fun&&) &")
   {
      SUBCASE("some")
      {
         maybe maybe_int = some(1);
         maybe maybe_float = maybe_int.and_then([](int val) -> maybe<float> {
            if (val == 1)
            {
               return some(static_cast<float>(val));
            }

            return none;
         });

         REQUIRE(maybe_int.is_some());
         CHECK(maybe_int == 1);

         REQUIRE(maybe_float.is_some());
         CHECK(maybe_float == 1.0f);
      }
      SUBCASE("become none")
      {
         maybe maybe_int = some(1);
         maybe maybe_float = maybe_int.and_then([](int val) -> maybe<float> {
            if (val != 1)
            {
               return some(static_cast<float>(val));
            }

            return none;
         });

         REQUIRE(maybe_int.is_some());
         CHECK(maybe_int == 1);

         CHECK(maybe_float.is_none());
      }
      SUBCASE("none")
      {
         maybe<int> maybe_none = none;
         maybe str_res = maybe_none.and_then([](int val) -> maybe<std::string> {
            return some(std::to_string(val));
         });

         CHECK(maybe_none.is_none());
         CHECK(str_res.is_none());
      }
   }
   TEST_CASE("and_then(fun&&) const&&")
   {
      auto default_int = [](int val) -> const maybe<int> {
         return some(std::move(val)); // NOLINT
      };

      auto default_none = []() -> const maybe<int> {
         return none;
      };

      SUBCASE("some")
      {
         const maybe maybe_float = default_int(1).and_then([](int val) {
            return val <= 1 ? maybe(some(1.0f)) : maybe<float>(none);
         });

         REQUIRE(maybe_float.is_some());
         CHECK(maybe_float == 1.0f);

         const maybe str_res = default_int(1).and_then([](int val) {
            return val <= 1 ? maybe(some(std::to_string(val))) : maybe<std::string>(none);
         });

         REQUIRE(str_res.is_some());
         CHECK(str_res == "1");
      }
      SUBCASE("become none")
      {
         const maybe maybe_float = default_int(2).and_then([](int val) {
            return val <= 1 ? maybe(some(1.0f)) : maybe<float>(none);
         });

         CHECK(maybe_float.is_none());

         const maybe str_res = default_int(2).and_then([](int val) {
            return val <= 1 ? maybe(some(std::to_string(val))) : maybe<std::string>(none);
         });

         CHECK(str_res.is_none());
      }
      SUBCASE("none")
      {
         const maybe maybe_float = default_none().and_then([](int val) {
            return val <= 1 ? maybe(some(1.0f)) : maybe<float>(none);
         });

         CHECK(maybe_float.is_none());

         const maybe str_res = default_none().and_then([](int val) {
            return val <= 1 ? maybe(some(std::to_string(val))) : maybe<std::string>(none);
         });

         CHECK(str_res.is_none());
      }
   }
   TEST_CASE("and_then(fun&&) &&")
   {
      SUBCASE("some")
      {
         const maybe maybe_float = maybe(some(1)).and_then([](int val) {
            return val <= 1 ? maybe(some(1.0f)) : maybe<float>(none);
         });

         REQUIRE(maybe_float.is_some());
         CHECK(maybe_float == 1.0f);

         const maybe str_res = maybe(some(1)).and_then([](int val) {
            return val <= 1 ? maybe(some(std::to_string(val))) : maybe<std::string>(none);
         });

         REQUIRE(str_res.is_some());
         CHECK(str_res == "1");
      }
      SUBCASE("become none")
      {
         const maybe maybe_float = maybe(some(2)).and_then([](int val) {
            return val <= 1 ? maybe(some(1.0f)) : maybe<float>(none);
         });

         CHECK(maybe_float.is_none());

         const maybe str_res = maybe(some(2)).and_then([](int val) {
            return val <= 1 ? maybe(some(std::to_string(val))) : maybe<std::string>(none);
         });

         CHECK(str_res.is_none());
      }
      SUBCASE("none")
      {
         const maybe maybe_float = maybe<int>(none).and_then([](int val) {
            return val <= 1 ? maybe(some(1.0f)) : maybe<float>(none);
         });

         CHECK(maybe_float.is_none());

         const maybe str_res = maybe<int>(none).and_then([](int val) {
            return val <= 1 ? maybe(some(std::to_string(val))) : maybe<std::string>(none);
         });

         CHECK(str_res.is_none());
      }
   }

   TEST_CASE("or_else(fun&&) const&")
   {
      SUBCASE("some")
      {
         const maybe int_zero = some(0);
         const maybe int_one = int_zero.or_else([]() -> maybe<int> {
            return some(1);
         });

         CHECK(int_zero == 0);
         CHECK(int_one == 0);

         const maybe str_hello = some(std::string("hello"));
         const maybe str_bye = str_hello.or_else([]() -> maybe<std::string> {
            return some(std::string("bye"));
         });

         CHECK(str_hello == "hello");
         CHECK(str_bye == "hello");
      }
      SUBCASE("none")
      {
         const maybe<int> int_zero = none;
         const maybe int_one = int_zero.or_else([]() -> maybe<int> {
            return some(1);
         });

         CHECK(int_zero.is_none());
         CHECK(int_one == 1);

         const maybe<std::string> str_hello = none;
         const maybe str_bye = str_hello.or_else([]() -> maybe<std::string> {
            return some(std::string("bye"));
         });

         CHECK(str_hello.is_none());
         CHECK(str_bye == "bye");
      }
   }
   TEST_CASE("or_else(fun&&) &")
   {
      SUBCASE("some")
      {
         maybe int_zero = some(0);
         maybe int_one = int_zero.or_else([]() -> maybe<int> {
            return some(1);
         });

         CHECK(int_zero == 0);
         CHECK(int_one == 0);

         maybe str_hello = some(std::string("hello"));
         maybe str_bye = str_hello.or_else([]() -> maybe<std::string> {
            return some(std::string("bye"));
         });

         CHECK(str_hello == "hello");
         CHECK(str_bye == "hello");
      }
      SUBCASE("none")
      {
         maybe<int> int_zero = none;
         maybe int_one = int_zero.or_else([]() -> maybe<int> {
            return some(1);
         });

         CHECK(int_zero.is_none());
         CHECK(int_one == 1);

         maybe<std::string> str_hello = none;
         maybe str_bye = str_hello.or_else([]() -> maybe<std::string> {
            return some(std::string("bye"));
         });

         CHECK(str_hello.is_none());
         CHECK(str_bye == "bye");
      }
   }
   TEST_CASE("or_else(fun&&) const&&")
   {
      const auto int_gen = [](int val) -> const maybe<int> {
         if (val == 0)
         {
            return some(0);
         }

         return none;
      };
      const auto str_gen = [](int val) -> const maybe<std::string> {
         if (val == 0)
         {
            return some(std::string("hello"));
         }

         return none;
      };

      SUBCASE("some")
      {
         maybe int_one = int_gen(0).or_else([]() -> maybe<int> {
            return some(1);
         });

         CHECK(int_one == 0);

         maybe str_bye = str_gen(0).or_else([]() -> maybe<std::string> {
            return some(std::string("bye"));
         });

         CHECK(str_bye == "hello");
      }
      SUBCASE("none")
      {
         maybe int_one = int_gen(1).or_else([]() -> maybe<int> {
            return some(1);
         });

         CHECK(int_one == 1);

         maybe str_bye = str_gen(1).or_else([]() -> maybe<std::string> {
            return some(std::string("bye"));
         });

         CHECK(str_bye == "bye");
      }
   }
   TEST_CASE("or_else(fun&&) &")
   {
      SUBCASE("some")
      {
         maybe int_one = maybe(some(0)).or_else([]() -> maybe<int> {
            return some(1);
         });

         CHECK(int_one == 0);

         maybe str_bye = maybe(some(std::string("hello"))).or_else([]() -> maybe<std::string> {
            return some(std::string("bye"));
         });

         CHECK(str_bye == "hello");
      }
      SUBCASE("none")
      {
         maybe int_one = maybe<int>(none).or_else([]() -> maybe<int> {
            return some(1);
         });

         CHECK(int_one == 1);

         maybe str_bye = maybe<std::string>(none).or_else([]() -> maybe<std::string> {
            return some(std::string("bye"));
         });

         CHECK(str_bye == "bye");
      }
   }

   TEST_CASE("transform_or_else(fun&&) const&")
   {
      SUBCASE("some")
      {
         const maybe int_val = some(0);
         const maybe float_res = int_val.transform_or_else(
            [](int val) {
               return maybe(some(static_cast<float>(val)));
            },
            [] {
               return maybe(some<float>(1.0f));
            });

         CHECK(float_res == 0.0f);

         const maybe str_val = some(std::string("hello"));
         const maybe str_res = str_val.transform_or_else(
            [](const std::string& str) {
               return maybe(some(str + " world"));
            },
            [] {
               return maybe(some<std::string>("bye"));
            });

         CHECK(str_res == "hello world");
      }
      SUBCASE("none")
      {
         const maybe<int> int_val = none;
         const maybe float_res = int_val.transform_or_else(
            [](int val) {
               return maybe(some(static_cast<float>(val)));
            },
            [] {
               return maybe(some<float>(1.0f));
            });

         CHECK(float_res == 1.0f);

         const maybe<std::string> str_val = none;
         const maybe str_res = str_val.transform_or_else(
            [](const std::string& str) {
               return maybe(some(str + " world"));
            },
            [] {
               return maybe(some<std::string>("bye"));
            });

         CHECK(str_res == "bye");
      }
   }
   TEST_CASE("transform_or_else(fun&&) &")
   {
      SUBCASE("some")
      {
         maybe int_val = some(0);
         const maybe float_res = int_val.transform_or_else(
            [](int val) {
               return maybe(some(static_cast<float>(val)));
            },
            [] {
               return maybe(some<float>(1.0f));
            });

         CHECK(float_res == 0.0f);

         maybe str_val = some(std::string("hello"));
         const maybe str_res = str_val.transform_or_else(
            [](const std::string& str) {
               return maybe(some(str + " world"));
            },
            [] {
               return maybe(some<std::string>("bye"));
            });

         CHECK(str_res == "hello world");
      }
      SUBCASE("none")
      {
         maybe<int> int_val = none;
         const maybe float_res = int_val.transform_or_else(
            [](int val) {
               return maybe(some(static_cast<float>(val)));
            },
            [] {
               return maybe(some<float>(1.0f));
            });

         CHECK(float_res == 1.0f);

         maybe<std::string> str_val = none;
         const maybe str_res = str_val.transform_or_else(
            [](const std::string& str) {
               return maybe(some(str + " world"));
            },
            [] {
               return maybe(some<std::string>("bye"));
            });

         CHECK(str_res == "bye");
      }
   }
   TEST_CASE("transform_or_else(fun&&) const&&")
   {
      const auto int_gen = [](int val) -> const maybe<int> {
         if (val == 0)
         {
            return some(0);
         }

         return none;
      };
      const auto str_gen = [](int val) -> const maybe<std::string> {
         if (val == 0)
         {
            return some(std::string("hello"));
         }

         return none;
      };

      SUBCASE("some")
      {
         const maybe float_res = int_gen(0).transform_or_else(
            [](int val) {
               return maybe(some(static_cast<float>(val)));
            },
            [] {
               return maybe(some<float>(1.0f));
            });

         CHECK(float_res == 0.0f);

         const maybe str_res = str_gen(0).transform_or_else(
            [](const std::string& str) {
               return maybe(some(str + " world"));
            },
            [] {
               return maybe(some<std::string>("bye"));
            });

         CHECK(str_res == "hello world");
      }
      SUBCASE("none")
      {
         const maybe float_res = int_gen(1).transform_or_else(
            [](int val) {
               return maybe(some(static_cast<float>(val)));
            },
            [] {
               return maybe(some<float>(1.0f));
            });

         CHECK(float_res == 1.0f);

         const maybe str_res = str_gen(1).transform_or_else(
            [](const std::string& str) {
               return maybe(some(str + " world"));
            },
            [] {
               return maybe(some<std::string>("bye"));
            });

         CHECK(str_res == "bye");
      }
   }
   TEST_CASE("transform_or_else(fun&&) &&")
   {
      SUBCASE("some")
      {
         const maybe float_res = maybe(some(0)).transform_or_else(
            [](int val) {
               return maybe(some(static_cast<float>(val)));
            },
            [] {
               return maybe(some<float>(1.0f));
            });

         CHECK(float_res == 0.0f);

         const maybe str_res = maybe(some(std::string("hello")))
                                  .transform_or_else(
                                     [](const std::string& str) {
                                        return maybe(some(str + " world"));
                                     },
                                     [] {
                                        return maybe(some<std::string>("bye"));
                                     });

         CHECK(str_res == "hello world");
      }
      SUBCASE("none")
      {
         const maybe float_res = maybe<int>(none).transform_or_else(
            [](int val) {
               return maybe(some(static_cast<float>(val)));
            },
            [] {
               return maybe(some<float>(1.0f));
            });

         CHECK(float_res == 1.0f);

         const maybe str_res = maybe<std::string>(none).transform_or_else(
            [](const std::string& str) {
               return maybe(some(str + " world"));
            },
            [] {
               return maybe(some<std::string>("bye"));
            });

         CHECK(str_res == "bye");
      }
   }

   TEST_CASE("operator==(const maybe<T>&, const maybe<U>&)")
   {
      const maybe maybe_int_1 = some(1);
      const maybe maybe_int_2 = some(2);
      const maybe maybe_int_3 = some(1);
      const maybe<int> maybe_none = none;

      CHECK(maybe_int_1 != maybe_int_2);
      CHECK(maybe_int_1 == maybe_int_3);
      CHECK(maybe_int_2 != maybe_int_3);
      CHECK(maybe_int_1 != maybe_none);
      CHECK(maybe_int_2 != maybe_none);
      CHECK(maybe_int_3 != maybe_none);
   }
   TEST_CASE("operator==(const maybe<T>&, none_t)")
   {
      const maybe maybe_int_1 = some(1);
      const maybe maybe_int_2 = some(2);
      const maybe<int> maybe_none = none;

      CHECK(maybe_int_1 != none);
      CHECK(maybe_int_2 != none);
      CHECK(maybe_none == none);
   }
   TEST_CASE("operator==(const maybe<T>&, U)")
   {
      const maybe maybe_int = some(1);
      const maybe maybe_float = some(1.0f);
      const maybe maybe_vector = some(std::vector({1, 1, 1}));

      CHECK(maybe_int == 1);
      CHECK(maybe_int != 0);
      CHECK(maybe_float == 1.0f);
      CHECK(maybe_float != 0.0f);
      CHECK(maybe_vector == std::vector({1, 1, 1}));
      CHECK(maybe_vector != std::vector({1, 1}));
   }

   TEST_CASE("operator<=>(const maybe<T>&, const maybe<U>&)")
   {
      const maybe int_zero = some(0);
      const maybe int_one = some(1);
      const maybe str_zero = some(std::string("hello"));
      const maybe str_one = some(std::string("hello1"));

      SUBCASE("<")
      {
         CHECK(int_zero < int_one);
         CHECK(str_zero < str_one);

         CHECK(not(int_one < int_zero)); // NOLINT
         CHECK(not(str_one < str_zero)); // NOLINT
      }
      SUBCASE("<=")
      {
         CHECK(int_zero <= int_one);
         CHECK(str_zero <= str_one);

         CHECK(not(int_one <= int_zero)); // NOLINT
         CHECK(not(str_one <= str_zero)); // NOLINT
      }
      SUBCASE(">")
      {
         CHECK(not(int_zero > int_one)); // NOLINT
         CHECK(not(str_zero > str_one)); // NOLINT

         CHECK(int_one > int_zero);
         CHECK(str_one > str_zero);
      }
      SUBCASE(">=")
      {
         CHECK(not(int_zero >= int_one)); // NOLINT
         CHECK(not(str_zero >= str_one)); // NOLINT

         CHECK(int_one >= int_zero);
         CHECK(str_one >= str_zero);
      }
   }
   TEST_CASE("operator<=>(const maybe<T>&, none_t)")
   {
      SUBCASE("<")
      {
         const maybe maybe_int = some(1);
         const maybe maybe_str = some(std::string("hello"));

         CHECK((maybe_int < none) != true); // NOLINT
         CHECK((maybe_str < none) != true); // NOLINT
      }
      SUBCASE("<=")
      {
         const maybe maybe_int = some(1);
         const maybe maybe_str = some(std::string("hello"));

         CHECK((maybe_int <= none) != true); // NOLINT
         CHECK((maybe_str <= none) != true); // NOLINT
      }
      SUBCASE(">")
      {
         const maybe maybe_int = some(1);
         const maybe maybe_str = some(std::string("hello"));

         CHECK((maybe_int > none) == true); // NOLINT
         CHECK((maybe_str > none) == true); // NOLINT
      }
      SUBCASE(">=")
      {
         const maybe maybe_int = some(1);
         const maybe maybe_str = some(std::string("hello"));

         CHECK((maybe_int >= none) == true); // NOLINT
         CHECK((maybe_str >= none) == true); // NOLINT
      }
   }
   TEST_CASE("operator<=>(const maybe<T>&, U)")
   {
      SUBCASE("<")
      {
         const maybe maybe_int = some(1);

         CHECK((maybe_int < 2) == true); // NOLINT
         CHECK((maybe_int < 1) != true); // NOLINT
         CHECK((maybe_int < 0) != true); // NOLINT

         const maybe maybe_str = some(std::string("hello"));

         CHECK((maybe_str < "hello1") == true); // NOLINT
         CHECK((maybe_str < "hello") != true);  // NOLINT
         CHECK((maybe_str < "hell") != true);   // NOLINT
      }
      SUBCASE("<=")
      {
         const maybe maybe_int = some(1);

         CHECK((maybe_int <= 2) == true); // NOLINT
         CHECK((maybe_int <= 1) == true); // NOLINT
         CHECK((maybe_int <= 0) != true); // NOLINT

         const maybe maybe_str = some(std::string("hello"));

         CHECK((maybe_str <= "hello1") == true); // NOLINT
         CHECK((maybe_str <= "hello") == true);  // NOLINT
         CHECK((maybe_str <= "hell") != true);   // NOLINT
      }
      SUBCASE(">")
      {
         const maybe maybe_int = some(1);

         CHECK((maybe_int > 2) != true); // NOLINT
         CHECK((maybe_int > 1) != true); // NOLINT
         CHECK((maybe_int > 0) == true); // NOLINT

         const maybe maybe_str = some(std::string("hello"));

         CHECK((maybe_str > "hello1") != true); // NOLINT
         CHECK((maybe_str > "hello") != true);  // NOLINT
         CHECK((maybe_str > "hell") == true);   // NOLINT
      }
      SUBCASE(">=")
      {
         const maybe maybe_int = some(1);

         CHECK((maybe_int >= 2) != true); // NOLINT
         CHECK((maybe_int >= 1) == true); // NOLINT
         CHECK((maybe_int >= 0) == true); // NOLINT

         const maybe maybe_str = some(std::string("hello"));

         CHECK((maybe_str >= "hello1") != true); // NOLINT
         CHECK((maybe_str >= "hello") == true);  // NOLINT
         CHECK((maybe_str >= "hell") == true);   // NOLINT
      }
   }

   TEST_CASE("std::swap(maybe&, maybe&)")
   {
      maybe int_zero = some(0);
      maybe int_one = some(1);

      CHECK(int_zero == 0);
      CHECK(int_one == 1);

      std::swap(int_zero, int_one);

      CHECK(int_zero == 1);
      CHECK(int_one == 0);

      maybe str_hello = some(std::string("hello"));
      maybe str_bye = some(std::string("bye"));

      CHECK(str_hello == "hello");
      CHECK(str_bye == "bye");

      std::swap(str_hello, str_bye);

      CHECK(str_hello == "bye");
      CHECK(str_bye == "hello");
   }
}
