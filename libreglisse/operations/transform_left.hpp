/**
 * @file operations/transform_left.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Wednesday, 30th of June 2021
 * @brief
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_OPERATIONS_TRANSFORM_LEFT_HPP
#define LIBREGLISSE_OPERATIONS_TRANSFORM_LEFT_HPP

#include <libreglisse/concepts.hpp>
#include <libreglisse/either.hpp>

#include <libreglisse/operations/pipe_closure.hpp>

namespace reglisse
{
   struct flat_transform_left_fn
   {
      template <typename LeftType, typename RightType, std::invocable<LeftType> Func>
      constexpr auto operator()(const either<LeftType, RightType>&& e, Func&& left_func)
      {
         using ret_t = either<std::invoke_result_t<Func, LeftType>, RightType>;

         if (e.is_left())
         {
            return ret_t(
               left(std::invoke(std::forward<Func>(left_func), std::move(e).take_left())));
         }

         return ret_t(right(std::move(e).take_right()));
      }
      template <typename LeftType, typename RightType, std::invocable<LeftType> Func>
      constexpr auto operator()(either<LeftType, RightType>&& e, Func&& left_func)
      {
         using ret_t = either<std::invoke_result_t<Func, LeftType>, RightType>;

         if (e.is_left())
         {
            return ret_t(
               left(std::invoke(std::forward<Func>(left_func), std::move(e).take_left())));
         }

         return ret_t(right(std::move(e).take_right()));
      }

      template <typename LeftType, typename RightType, std::invocable<LeftType> Func>
      constexpr auto operator()(const either<LeftType, RightType>& e, Func&& left_func)
      {
         using ret_t = either<std::invoke_result_t<Func, LeftType>, RightType>;

         if (e.is_left())
         {
            return ret_t(left(std::invoke(std::forward<Func>(left_func), e.borrow_left())));
         }

         return ret_t(right(e.borrow_right()));
      }
      template <typename LeftType, typename RightType, std::invocable<LeftType> Func>
      constexpr auto operator()(either<LeftType, RightType>& e, Func&& left_func)
      {
         using ret_t = either<std::invoke_result_t<Func, LeftType>, RightType>;

         if (e.is_left())
         {
            return ret_t(left(std::invoke(std::forward<Func>(left_func), e.borrow_left())));
         }

         return ret_t(right(e.borrow_right()));
      }
   };

   const constexpr operation<flat_transform_left_fn> transform_left = {};
} // namespace reglisse

#endif // LIBREGLISSE_OPERATIONS_TRANSFORM_LEFT_HPP
