/**
 * @file operations/and_then.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Wednesday, 30th of June 2021
 * @brief Contains the 'and_then' operation
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_OPERATIONS_AND_THEN_HPP
#define LIBREGLISSE_OPERATIONS_AND_THEN_HPP

#include <libreglisse/concepts.hpp>
#include <libreglisse/maybe.hpp>
#include <libreglisse/result.hpp>

#include <libreglisse/operations/pipe_closure.hpp>

namespace reglisse::inline v1
{
   namespace detail
   {
      template <typename ValueType, typename Func>
      concept and_then_returns_maybe = maybe_monad<std::invoke_result_t<Func, ValueType>>;
   } // namespace detail

   /**
    * @brief Functor used to implement the 'and_then' operation on maybe & result monads
    */
   struct and_then_fn
   {
      template <typename ValueType, std::invocable<ValueType> Func>
         requires detail::and_then_returns_maybe<ValueType, Func>
      constexpr auto operator()(const maybe<ValueType>&& m, Func some_func) const
         -> std::invoke_result_t<Func, ValueType>
      {
         if (m.is_some())
         {
            return std::invoke(std::forward<Func>(some_func), std::move(m).take());
         }

         return none;
      }
      template <typename ValueType, std::invocable<ValueType> Func>
         requires detail::and_then_returns_maybe<ValueType, Func>
      constexpr auto operator()(maybe<ValueType>&& m, Func some_func) const
         -> std::invoke_result_t<Func, ValueType>
      {
         if (m.is_some())
         {
            return std::invoke(std::forward<Func>(some_func), std::move(m).take());
         }

         return none;
      }
      template <typename ValueType, std::invocable<ValueType> Func>
         requires detail::and_then_returns_maybe<ValueType, Func>
      constexpr auto operator()(const maybe<ValueType>& m, Func some_func) const
         -> std::invoke_result_t<Func, ValueType>
      {
         if (m.is_some())
         {
            return std::invoke(std::forward<Func>(some_func), m.borrow());
         }

         return none;
      }
      template <typename ValueType, std::invocable<ValueType> Func>
         requires detail::and_then_returns_maybe<ValueType, Func>
      constexpr auto operator()(maybe<ValueType>& m, Func some_func) const
         -> std::invoke_result_t<Func, ValueType>
      {
         if (m.is_some())
         {
            return std::invoke(std::forward<Func>(some_func), m.borrow());
         }

         return none;
      }

      template <typename ValueType, typename ErrorType, std::invocable<ValueType> Func>
      constexpr auto operator()(const result<ValueType, ErrorType>&& r, Func&& value_func) const
      {
         using res_t = std::invoke_result_t<Func, ValueType>;

         if (r.is_ok())
         {
            return std::invoke(std::forward<Func>(value_func), std::move(r).take());
         }

         return res_t(err(std::move(r).take_err()));
      }
      template <typename ValueType, typename ErrorType, std::invocable<ValueType> Func>
      constexpr auto operator()(result<ValueType, ErrorType>&& r, Func&& value_func) const
      {
         using res_t = std::invoke_result_t<Func, ValueType>;

         if (r.is_ok())
         {
            return std::invoke(std::forward<Func>(value_func), std::move(r).take());
         }

         return res_t(err(std::move(r).take_err()));
      }
      template <typename ValueType, typename ErrorType, std::invocable<ValueType> Func>
      constexpr auto operator()(const result<ValueType, ErrorType>& r, Func&& value_func) const
      {
         using res_t = std::invoke_result_t<Func, ValueType>;

         if (r.is_ok())
         {
            return std::invoke(std::forward<Func>(value_func), r.borrow());
         }

         return res_t(err(r.borrow_err()));
      }
      template <typename ValueType, typename ErrorType, std::invocable<ValueType> Func>
      constexpr auto operator()(result<ValueType, ErrorType>& r, Func&& value_func) const
      {
         using res_t = std::invoke_result_t<Func, ValueType>;

         if (r.is_ok())
         {
            return std::invoke(std::forward<Func>(value_func), r.borrow());
         }

         return res_t(err(r.borrow_err()));
      }
   };

   /**
    * @brief The 'and_then' operation used on maybe & result.
    */
   const constexpr operation<and_then_fn> and_then = {};
} // namespace reglisse

#endif // LIBREGLISSE_OPERATIONS_AND_THEN_HPP
