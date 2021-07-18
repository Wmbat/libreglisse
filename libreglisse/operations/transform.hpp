/**
 * @file operations/transform.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Wednesday, 30th of June 2021
 * @brief Contains the 'transform' operation
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_OPERATIONS_TRANSFORM_HPP
#define LIBREGLISSE_OPERATIONS_TRANSFORM_HPP

#include <libreglisse/concepts.hpp>
#include <libreglisse/maybe.hpp>
#include <libreglisse/result.hpp>

#include <libreglisse/operations/pipe_closure.hpp>

namespace reglisse
{
   /**
    * @brief Functor used to implement the 'transform' operation on maybe & result monads
    */
   struct transform_fn
   {
      template <typename ValueType, std::invocable<ValueType> Func>
      constexpr auto operator()(const maybe<ValueType>&& m, Func&& ok_func)
      {
         using res_t = maybe<std::invoke_result_t<Func, ValueType>>;

         if (m.is_some())
         {
            return res_t(some(std::invoke(std::forward<Func>(ok_func), std::move(m).take())));
         }

         return res_t(none);
      }
      template <typename ValueType, std::invocable<ValueType> Func>
      constexpr auto operator()(maybe<ValueType>&& m, Func&& ok_func)
      {
         using res_t = maybe<std::invoke_result_t<Func, ValueType>>;

         if (m.is_some())
         {
            return res_t(some(std::invoke(std::forward<Func>(ok_func), std::move(m).take())));
         }

         return res_t(none);
      }
      template <typename ValueType, std::invocable<ValueType> Func>
      constexpr auto operator()(const maybe<ValueType>& m, Func&& ok_func)
      {
         using res_t = maybe<std::invoke_result_t<Func, ValueType>>;

         if (m.is_some())
         {
            return res_t(some(std::invoke(std::forward<Func>(ok_func), m.borrow())));
         }

         return res_t(none);
      }
      template <typename ValueType, std::invocable<ValueType> Func>
      constexpr auto operator()(maybe<ValueType>& m, Func&& ok_func)
      {
         using res_t = maybe<std::invoke_result_t<Func, ValueType>>;

         if (m.is_some())
         {
            return res_t(some(std::invoke(std::forward<Func>(ok_func), m.borrow())));
         }

         return res_t(none);
      }

      template <typename OkType, typename ErrType, std::invocable<OkType> Func>
      constexpr auto operator()(const result<OkType, ErrType>&& r, Func&& ok_func)
      {
         using res_t = result<std::invoke_result_t<Func, OkType>, ErrType>;

         if (r.is_ok())
         {
            return res_t(ok(std::invoke(std::forward<Func>(ok_func), std::move(r).take())));
         }

         return res_t(err(std::move(r).take_err()));
      }
      template <typename OkType, typename ErrType, std::invocable<OkType> Func>
      constexpr auto operator()(result<OkType, ErrType>&& r, Func&& ok_func)
      {
         using res_t = result<std::invoke_result_t<Func, OkType>, ErrType>;

         if (r.is_ok())
         {
            return res_t(ok(std::invoke(std::forward<Func>(ok_func), std::move(r).take())));
         }

         return res_t(err(std::move(r).take_err()));
      }

      template <typename OkType, typename ErrType, std::invocable<OkType> Func>
      constexpr auto operator()(const result<OkType, ErrType>& r, Func&& ok_func)
      {
         using res_t = result<std::invoke_result_t<Func, OkType>, ErrType>;

         if (r.is_ok())
         {
            return res_t(ok(std::invoke(std::forward<Func>(ok_func), r.borrow())));
         }

         return res_t(err(r.borrow_err()));
      }
      template <typename OkType, typename ErrType, std::invocable<OkType> Func>
      constexpr auto operator()(result<OkType, ErrType>& r, Func&& ok_func)
      {
         using res_t = result<std::invoke_result_t<Func, OkType>, ErrType>;

         if (r.is_ok())
         {
            return res_t(ok(std::invoke(std::forward<Func>(ok_func), r.borrow())));
         }

         return res_t(err(r.borrow_err()));
      }
   };

   /**
    * @brief Piping operation to transform the data contained in a maybe or result monad.
    */
   const constexpr operation<transform_fn> transform = {};
} // namespace reglisse

#endif // LIBREGLISSE_OPERATIONS_TRANSFORM_HPP
