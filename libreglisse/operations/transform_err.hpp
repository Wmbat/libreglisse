/**
 * @file operations/transform_err.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Wednesday, 30th of June 2021
 * @brief Contains the 'transform_err' operation
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_OPERATIONS_TRANSFORM_ERR_HPP
#define LIBREGLISSE_OPERATIONS_TRANSFORM_ERR_HPP

#include <libreglisse/concepts.hpp>
#include <libreglisse/result.hpp>

#include <libreglisse/operations/pipe_closure.hpp>

namespace reglisse::inline v0
{
   /**
    * @brief Functor used to implement the 'transform_err' operation on maybe & result monads
    */
   struct transform_err_fn
   {
      template <typename OkType, typename ErrType, std::invocable<ErrType> Func>
      constexpr auto operator()(const result<OkType, ErrType>&& r, Func&& err_func)
         -> result<OkType, std::invoke_result_t<Func, ErrType>>
      {
         if (r.is_err())
         {
            return err(std::invoke(std::forward<Func>(err_func), std::move(r).take_err()));
         }

         return ok(std::move(r).take());
      }
      template <typename OkType, typename ErrType, std::invocable<ErrType> Func>
      constexpr auto operator()(result<OkType, ErrType>&& r, Func&& err_func)
         -> result<OkType, std::invoke_result_t<Func, ErrType>>
      {
         if (r.is_err())
         {
            return err(std::invoke(std::forward<Func>(err_func), std::move(r).take_err()));
         }

         return ok(std::move(r).take());
      }
      template <typename OkType, typename ErrType, std::invocable<ErrType> Func>
      constexpr auto operator()(const result<OkType, ErrType>& r, Func&& err_func)
         -> result<OkType, std::invoke_result_t<Func, ErrType>>
      {
         if (r.is_err())
         {
            return err(std::invoke(std::forward<Func>(err_func), r.borrow_err()));
         }

         return ok(r.borrow());
      }
      template <typename OkType, typename ErrType, std::invocable<ErrType> Func>
      constexpr auto operator()(result<OkType, ErrType>& r, Func&& err_func)
         -> result<OkType, std::invoke_result_t<Func, ErrType>>
      {
         if (r.is_err())
         {
            return err(std::invoke(std::forward<Func>(err_func), r.borrow_err()));
         }

         return ok(r.borrow);
      }
   };

   /**
    * @brief Piping operation to transform the error contained in a result monad.
    */
   const constexpr operation<transform_err_fn> transform_err = {};
} // namespace reglisse::v0

#endif // LIBREGLISSE_OPERATIONS_TRANSFORM_ERR_HPP
