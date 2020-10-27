#pragma once

#include <concepts>
#include <type_traits>

namespace monad
{
   // clang-format off
   template <typename Any>
   concept trivially_default_constructible = std::is_trivially_default_constructible_v<Any>;

   template <typename Any>
   concept trivially_copyable = std::is_trivially_copyable_v<Any>;

   template <typename Any>
   concept trivially_destructible = std::is_trivially_destructible_v<Any>;

   template <typename Any>
   concept trivial = 
      trivially_default_constructible<Any> &&
      trivially_copyable<Any> &&  
      trivially_destructible<Any>;
   // clang-format on
} // namespace monad
