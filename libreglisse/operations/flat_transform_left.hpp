/**
 * @file operations/flat_transform_left.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Wednesday, 30th of June 2021
 * @brief
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_OPERATIONS_FLAT_TRANSFORM_LEFT_HPP
#define LIBREGLISSE_OPERATIONS_FLAT_TRANSFORM_LEFT_HPP

#include <libreglisse/concepts.hpp>
#include <libreglisse/either.hpp>

#include <libreglisse/operations/pipe_closure.hpp>

namespace reglisse
{
   namespace detail
   {
      template <typename LeftType, typename RightType, typename Func>
      concept ensure_ftransforml_returns_valid_either =
         either_monad<std::invoke_result_t<Func, LeftType>> and
         std::same_as<typename std::invoke_result_t<Func, LeftType>::right_type, RightType>;
   } // namespace detail

   struct flat_transform_left_fn
   {
      template <typename LeftType, typename RightType, std::invocable<LeftType> Func>
         requires detail::ensure_ftransforml_returns_valid_either<LeftType, RightType, Func>
      constexpr auto operator()(const either<LeftType, RightType>&& e, Func&& left_func)
      {
         using ret_t = std::invoke_result_t<Func, LeftType>;

         if (e.is_left())
         {
            return std::invoke(std::forward<Func>(left_func), std::move(e).take_left());
         }

         return ret_t(right(std::move(e).take_right()));
      }
      template <typename LeftType, typename RightType, std::invocable<LeftType> Func>
         requires detail::ensure_ftransforml_returns_valid_either<LeftType, RightType, Func>
      constexpr auto operator()(either<LeftType, RightType>&& e, Func&& left_func)
      {
         using ret_t = std::invoke_result_t<Func, LeftType>;

         if (e.is_left())
         {
            return std::invoke(std::forward<Func>(left_func), std::move(e).take_left());
         }

         return ret_t(right(std::move(e).take_right()));
      }

      template <typename LeftType, typename RightType, std::invocable<LeftType> Func>
         requires detail::ensure_ftransforml_returns_valid_either<LeftType, RightType, Func>
      constexpr auto operator()(const either<LeftType, RightType>& e, Func&& left_func)
      {
         using ret_t = std::invoke_result_t<Func, LeftType>;

         if (e.is_left())
         {
            return std::invoke(std::forward<Func>(left_func), e.borrow_left());
         }

         return ret_t(right(e.borrow_right()));
      }
      template <typename LeftType, typename RightType, std::invocable<LeftType> Func>
         requires detail::ensure_ftransforml_returns_valid_either<LeftType, RightType, Func>
      constexpr auto operator()(either<LeftType, RightType>& e, Func&& left_func)
      {
         using ret_t = std::invoke_result_t<Func, LeftType>;

         if (e.is_left())
         {
            return std::invoke(std::forward<Func>(left_func), e.borrow_left());
         }

         return ret_t(right(e.borrow_right()));
      }
   };

   const constexpr operation<flat_transform_left_fn> flat_transform_left = {};
} // namespace reglisse

#endif // LIBREGLISSE_OPERATIONS_FLAT_TRANSFORM_LEFT_HPP
