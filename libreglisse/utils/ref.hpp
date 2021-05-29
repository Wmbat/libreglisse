/**
 * @file utils/ref.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Saturday, 22nd of may 2021
 * @brief Contains the in_place_t struct.
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_UTILS_REF_HPP
#define LIBREGLISSE_UTILS_REF_HPP

#include <functional>

namespace reglisse
{
   template <typename Any>
   using ref = std::reference_wrapper<Any>;

   template <typename T>
   using const_ref = std::reference_wrapper<std::add_const_t<std::remove_reference_t<T>>>;

   template <typename T>
   using mut_ref = std::reference_wrapper<std::remove_const_t<std::remove_reference_t<T>>>;
} // namespace reglisse

#endif // LIBREGLISSE_UTILS_REF_HPP
