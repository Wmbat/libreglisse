/**
 * @file operations/or_else.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Wednesday, 30th of June 2021
 * @brief
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_OPERATIONS_OR_ELSE_HPP
#define LIBREGLISSE_OPERATIONS_OR_ELSE_HPP

#include <libreglisse/concepts.hpp>
#include <libreglisse/maybe.hpp>
#include <libreglisse/result.hpp>

#include <libreglisse/operations/pipe_closure.hpp>

#include <type_traits>

namespace reglisse
{
   namespace detail
   {
      template <typename Func, typename ValueType>
      concept ensure_or_else_returns_valid_maybe = maybe_monad<std::invoke_result_t<Func>> and
         std::same_as<typename std::invoke_result_t<Func>::value_type, ValueType>;

      template <typename Func, typename ValueType, typename ErrorType>
      concept ensure_or_else_returns_valid_result =
         result_monad<std::invoke_result_t<Func, ErrorType>> and
         std::same_as<typename std::invoke_result_t<Func, ErrorType>::value_type, ValueType>;
   } // namespace detail

   struct or_else_fn
   {
      template <typename ValueType, std::invocable Func>
         requires detail::ensure_or_else_returns_valid_maybe<Func, ValueType>
      constexpr auto operator()(const maybe<ValueType>&& m, Func none_func) const
         -> maybe<ValueType>
      {
         if (m.is_some())
         {
            return some(std::move(m).take());
         }

         return std::invoke(std::forward<Func>(none_func));
      }
      template <typename ValueType, std::invocable Func>
         requires detail::ensure_or_else_returns_valid_maybe<Func, ValueType>
      constexpr auto operator()(maybe<ValueType>&& m, Func none_func) const -> maybe<ValueType>
      {
         if (m.is_some())
         {
            return some(std::move(m).take());
         }

         return std::invoke(std::forward<Func>(none_func));
      }
      template <typename ValueType, std::invocable Func>
         requires detail::ensure_or_else_returns_valid_maybe<Func, ValueType>
      constexpr auto operator()(const maybe<ValueType>& m, Func none_func) const -> maybe<ValueType>
      {
         if (m.is_some())
         {
            return some(m.borrow());
         }

         return std::invoke(std::forward<Func>(none_func));
      }
      template <typename ValueType, std::invocable Func>
         requires detail::ensure_or_else_returns_valid_maybe<Func, ValueType>
      constexpr auto operator()(maybe<ValueType>& m, Func none_func) const -> maybe<ValueType>
      {
         if (m.is_some())
         {
            return some(m.borrow());
         }

         return std::invoke(std::forward<Func>(none_func));
      }

      template <typename ValueType, typename ErrorType, std::invocable<ErrorType> Func>
         requires detail::ensure_or_else_returns_valid_result<Func, ValueType, ErrorType>
      auto operator()(result<ValueType, ErrorType>&& r, Func&& err_func)
      {
         using res_t = std::invoke_result_t<Func, ErrorType>;

         if (r.is_ok())
         {
            return res_t(ok(std::move(r).take()));
         }

         return std::invoke(std::forward<Func>(err_func), std::move(r).take_err());
      }
      template <typename ValueType, typename ErrorType, std::invocable<ErrorType> Func>
         requires detail::ensure_or_else_returns_valid_result<Func, ValueType, ErrorType>
      auto operator()(const result<ValueType, ErrorType>&& r, Func&& err_func)
      {
         using res_t = std::invoke_result_t<Func, ErrorType>;

         if (r.is_ok())
         {
            return res_t(ok(std::move(r).take()));
         }

         return std::invoke(std::forward<Func>(err_func), std::move(r).take_err());
      }
      template <typename ValueType, typename ErrorType, std::invocable<ErrorType> Func>
         requires detail::ensure_or_else_returns_valid_result<Func, ValueType, ErrorType>
      auto operator()(const result<ValueType, ErrorType>& r, Func&& err_func)
      {
         using res_t = std::invoke_result_t<Func, ErrorType>;

         if (r.is_ok())
         {
            return res_t(ok(r.borrow()));
         }

         return std::invoke(std::forward<Func>(err_func), r.borrow_err());
      }
      template <typename ValueType, typename ErrorType, std::invocable<ErrorType> Func>
         requires detail::ensure_or_else_returns_valid_result<Func, ValueType, ErrorType>
      auto operator()(result<ValueType, ErrorType>& r, Func&& err_func)
      {
         using res_t = std::invoke_result_t<Func, ErrorType>;

         if (r.is_ok())
         {
            return res_t(ok(r.borrow()));
         }

         return std::invoke(std::forward<Func>(err_func), r.borrow_err());
      }
   };

   const constexpr operation<or_else_fn> or_else = {};
} // namespace reglisse

#endif // LIBREGLISSE_OPERATIONS_OR_ELSE_HPP
