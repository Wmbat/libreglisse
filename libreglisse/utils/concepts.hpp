/**
 * @file utils/in_place.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Saturday, 22nd of may 2021
 * @brief Contains the in_place_t struct.
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_UTILS_CONCEPTS_HPP
#define LIBREGLISSE_UTILS_CONCEPTS_HPP

#include <concepts>
#include <type_traits>

namespace reglisse
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
} // namespace reglisse

#endif // LIBREGLISSE_UTILS_CONCEPTS_HPP
