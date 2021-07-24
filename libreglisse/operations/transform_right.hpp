/**
 * @file operations/transform_right.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Wednesday, 30th of June 2021
 * @brief Contains the 'transform_right' operation
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_OPERATIONS_TRANSFORM_RIGHT_HPP
#define LIBREGLISSE_OPERATIONS_TRANSFORM_RIGHT_HPP

#include <libreglisse/concepts.hpp>
#include <libreglisse/either.hpp>

#include <libreglisse/operations/pipe_closure.hpp>

namespace reglisse::inline v1
{
   /**
    * @brief Functor used to implement the 'transform_left' operation on either monads
    */
   struct transform_right_fn
   {
      template <typename LeftType, typename RightType, std::invocable<RightType> Func>
      constexpr auto operator()(const either<LeftType, RightType>&& e, Func right_func)
      {
         using ret_t = either<LeftType, std::invoke_result_t<Func, RightType>>;

         if (e.is_right())
         {
            return ret_t(
               right(std::invoke(std::forward<Func>(right_func), std::move(e).take_right())));
         }

         return ret_t(left(e.take_left()));
      }
      template <typename LeftType, typename RightType, std::invocable<RightType> Func>
      constexpr auto operator()(either<LeftType, RightType>&& e, Func right_func)
      {
         using ret_t = either<LeftType, std::invoke_result_t<Func, RightType>>;

         if (e.is_right())
         {
            return ret_t(
               right(std::invoke(std::forward<Func>(right_func), std::move(e).take_right())));
         }

         return ret_t(left(e.take_left()));
      }
      template <typename LeftType, typename RightType, std::invocable<RightType> Func>
      constexpr auto operator()(const either<LeftType, RightType>& e, Func right_func)
      {
         using ret_t = either<LeftType, std::invoke_result_t<Func, RightType>>;

         if (e.is_right())
         {
            return ret_t(right(std::invoke(std::forward<Func>(right_func), e.borrow_right())));
         }

         return ret_t(left(e.borrow_left()));
      }
      template <typename LeftType, typename RightType, std::invocable<RightType> Func>
      constexpr auto operator()(either<LeftType, RightType>& e, Func right_func)
      {
         using ret_t = either<LeftType, std::invoke_result_t<Func, RightType>>;

         if (e.is_right())
         {
            return ret_t(right(std::invoke(std::forward<Func>(right_func), e.borrow_right())));
         }

         return ret_t(left(e.borrow_left()));
      }
   };

   const constexpr operation<transform_right_fn> transform_right = {};
} // namespace reglisse

#endif // LIBREGLISSE_OPERATIONS_TRANSFORM_RIGHT_HPP
