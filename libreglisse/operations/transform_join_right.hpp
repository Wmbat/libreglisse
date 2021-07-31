/**
 * @file operations/flat_transform_right.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Wednesday, 30th of June 2021
 * @brief Contains the 'flat_transform_right' operation
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_OPERATIONS_FLAT_TRANSFORM_LEFT_HPP
#define LIBREGLISSE_OPERATIONS_FLAT_TRANSFORM_LEFT_HPP

#include <libreglisse/concepts.hpp>
#include <libreglisse/either.hpp>

#include <libreglisse/operations/pipe_closure.hpp>

namespace reglisse::inline v0
{
   namespace detail
   {
      template <typename LeftType, typename RightType, typename Func>
      concept ensure_transformjr_returns_valid_either =
         either_monad<std::invoke_result_t<Func, RightType>> and
         std::same_as<typename std::invoke_result_t<Func, RightType>::left_type, LeftType>;
   } // namespace detail

   /**
    * @brief Functor used to implement the 'flat_transform_right' operation on either monads
    */
   struct transform_join_right_fn
   {
      template <typename LeftType, typename RightType, std::invocable<RightType> Func>
         requires detail::ensure_transformjr_returns_valid_either<LeftType, RightType, Func>
      constexpr auto operator()(const either<LeftType, RightType>&& e, Func&& right_func)
      {
         using ret_t = std::invoke_result_t<Func, RightType>;

         if (e.is_right())
         {
            return std::invoke(std::forward<Func>(right_func), std::move(e).take_right());
         }

         return ret_t(left(std::move(e).take_left()));
      }
      template <typename LeftType, typename RightType, std::invocable<RightType> Func>
         requires detail::ensure_transformjr_returns_valid_either<LeftType, RightType, Func>
      constexpr auto operator()(either<LeftType, RightType>&& e, Func&& right_func)
      {
         using ret_t = std::invoke_result_t<Func, RightType>;

         if (e.is_right())
         {
            return std::invoke(std::forward<Func>(right_func), std::move(e).take_right());
         }

         return ret_t(left(std::move(e).take_left()));
      }

      template <typename LeftType, typename RightType, std::invocable<RightType> Func>
         requires detail::ensure_transformjr_returns_valid_either<LeftType, RightType, Func>
      constexpr auto operator()(const either<LeftType, RightType>& e, Func&& right_func)
      {
         using ret_t = std::invoke_result_t<Func, RightType>;

         if (e.is_right())
         {
            return std::invoke(std::forward<Func>(right_func), e.borrow_right());
         }

         return ret_t(left(e.borrow_left()));
      }
      template <typename LeftType, typename RightType, std::invocable<RightType> Func>
         requires detail::ensure_transformjr_returns_valid_either<LeftType, RightType, Func>
      constexpr auto operator()(either<LeftType, RightType>& e, Func&& right_func)
      {
         using ret_t = std::invoke_result_t<Func, RightType>;

         if (e.is_right())
         {
            return std::invoke(std::forward<Func>(right_func), e.borrow_right());
         }

         return ret_t(left(e.borrow_left()));
      }
   };

   const constexpr operation<transform_join_right_fn> transform_join_right = {};
} // namespace reglisse::v0

#endif // LIBREGLISSE_OPERATIONS_FLAT_TRANSFORM_LEFT_HPP
