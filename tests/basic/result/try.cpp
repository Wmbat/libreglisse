#define LIBREGLISSE_USE_EXCEPTIONS

#include <libreglisse/try.hpp>

#include <catch2/catch.hpp>

#include <stdexcept>

using namespace reglisse;

auto throwing_fun() -> int
{
   throw std::runtime_error("hello");
}

SCENARIO("Wrapping exceptions in a result", "[result]")
{
   GIVEN("A paramless function that throws")
   {
      auto res = try_wrap<std::runtime_error>(throwing_fun);

      THEN("The result should contain an error") { CHECK(res.is_err()); }
   }
}
