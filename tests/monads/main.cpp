#include <monads/either.hpp>
#include <monads/maybe.hpp>

#include <gtest/gtest.h>

auto main(int argc, char* argv[]) -> int
{
   testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
