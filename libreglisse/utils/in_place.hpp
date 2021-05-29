/**
 * @file utils/in_place.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Saturday, 22nd of may 2021
 * @brief Contains the in_place_t struct.
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_UTILS_IN_PLACE_HPP
#define LIBREGLISSE_UTILS_IN_PLACE_HPP

namespace reglisse
{
   /**
    * @brief
    */
   struct in_place_t
   {
      in_place_t() noexcept = default;
   };

   /**
    * @brief
    */
   static constexpr in_place_t in_place;
} // namespace reglisse

#endif // LIBREGLISSE_UTILS_IN_PLACE_HPP
