/**
 * @file result.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Monday, 7th of june 2021
 * @brief Contains everything related to the result monad
 * @copyright Copyright (C) 2021 wmbat.
 */

#pragma once

#include <libreglisse/concepts.hpp>

#if defined(LIBREGLISSE_USE_EXCEPTIONS)
#   include <libreglisse/detail/invalid_access_exception.hpp>
#else
#   include <cassert>
#endif // defined (LIBREGLISSE_USE_EXCEPTIONS)

#include <functional>
#include <memory>

namespace reglisse::detail
{
   inline void handle_invalid_value_result_access(bool check)
   {
#if defined(LIBREGLISSE_USE_EXCEPTIONS)
      if (!check)
      {
         throw invalid_access_exception("result currently holds an error");
      }
#else
      assert(check && "result currently holds an error"); // NOLINT
#endif // defined(LIBREGLISSE_USE_EXCEPTIONS)
   }

   inline void handle_invalid_error_result_access(bool check)
   {
#if defined(LIBREGLISSE_USE_EXCEPTIONS)
      if (!check)
      {
         throw invalid_access_exception("result currently holds an value");
      }
#else
      assert(check && "result currently holds a value");  // NOLINT
#endif // defined(LIBREGLISSE_USE_EXCEPTIONS)
   }
} // namespace reglisse::detail

namespace reglisse
{
   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class ok;

   /**
    * @brief Helper class to construct a result containing an error.
    */
   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class err
   {
   public:
      using value_type = T;

   public:
      explicit constexpr err(const value_type& value) : m_value(value) {}
      explicit constexpr err(value_type&& value) : m_value(std::move(value)) {}

      constexpr auto value() const& noexcept -> const value_type& { return m_value; }
      constexpr auto value() & noexcept -> value_type& { return m_value; }
      constexpr auto value() const&& noexcept -> const value_type { return std::move(m_value); }
      constexpr auto value() && noexcept -> value_type { return std::move(m_value); }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const err<U>& rhs) const -> bool
      {
         return value() == rhs.value();
      }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const ok<U>& rhs) const -> bool
      {
         return value() == rhs.value();
      }

   private:
      value_type m_value;
   };

   /**
    * @brief Helper class to construct a result containing a value.
    */
   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class ok
   {
   public:
      using value_type = T;

   public:
      explicit constexpr ok(const value_type& value) : m_value(value) {}
      explicit constexpr ok(value_type&& value) : m_value(std::move(value)) {}

      constexpr auto value() const& noexcept -> const value_type& { return m_value; }
      constexpr auto value() & noexcept -> value_type& { return m_value; }
      constexpr auto value() const&& noexcept -> const value_type { return std::move(m_value); }
      constexpr auto value() && noexcept -> value_type { return std::move(m_value); }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const ok<U>& rhs) const -> bool
      {
         return value() == rhs.value();
      }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const err<U>& rhs) const -> bool
      {
         return value() == rhs.value();
      }

   private:
      value_type m_value;
   };

   err(const char*)->err<std::string>;
   ok(const char*)->ok<std::string>;

   /**
    * @brief A monadic type that container either a normal value or an error.
    */
   template <std::movable ValueType, std::movable ErrorType>
      requires(not(std::is_reference_v<ValueType> or std::is_reference_v<ErrorType>))
   class result
   {
   public:
      using value_type = ValueType;
      using error_type = ErrorType;

   private:
      template <class value_fun, class error_fun>
      using join_result = std::common_type_t<std::invoke_result_t<value_fun, value_type>,
                                             std::invoke_result_t<error_fun, error_type>>;

   public:
      constexpr result() = delete;
      constexpr result(ok<value_type>&& value) : m_is_ok(true)
      {
         std::construct_at(&m_value, std::move(value).value()); // NOLINT
      }
      constexpr result(err<error_type>&& error) : m_is_ok(false)
      {
         std::construct_at(&m_error, std::move(error).value()); // NOLINT
      }
      constexpr result(const result& other) : m_is_ok(other.m_is_ok)
      {
         if (is_ok())
         {
            std::construct_at(&m_value, other.borrow()); // NOLINT
         }
         else
         {
            std::construct_at(&m_error, other.borrow_err()); // NOLINT
         }
      }
      constexpr result(result&& other) noexcept : m_is_ok(other.m_is_ok)
      {
         if (is_ok())
         {
            std::construct_at(&m_value, std::move(other).take()); // NOLINT
         }
         else
         {
            std::construct_at(&m_error, std::move(other).take_err()); // NOLINT
         }
      }
      constexpr ~result()
      {
         if (is_ok())
         {
            std::destroy_at(&m_value); // NOLINT
         }
         else
         {
            std::destroy_at(&m_error); // NOLINT
         }
      }

      constexpr auto operator=(const result& rhs) -> result&
      {
         if (this != &rhs)
         {
            if (is_ok())
            {
               std::destroy_at(&m_value); // NOLINT
            }
            else
            {
               std::destroy_at(&m_error); // NOLINT
            }

            m_is_ok = rhs.m_is_ok;

            if (is_ok())
            {
               std::construct_at(&m_value, rhs.borrow()); // NOLINT
            }
            else
            {
               std::construct_at(&m_error, rhs.borrow_err()); // NOLINT
            }
         }

         return *this;
      }
      constexpr auto operator=(result&& rhs) noexcept -> result&
      {
         if (this != &rhs)
         {
            if (is_ok())
            {
               std::destroy_at(&m_value); // NOLINT
            }
            else
            {
               std::destroy_at(&m_error); // NOLINT
            }

            m_is_ok = rhs.m_is_ok;

            if (is_ok())
            {
               std::construct_at(&m_value, std::move(rhs).take()); // NOLINT
            }
            else
            {
               std::construct_at(&m_error, std::move(rhs).take_err()); // NOLINT
            }
         }

         return *this;
      }

      constexpr auto borrow() const& -> const value_type&
      {
         detail::handle_invalid_value_result_access(is_ok());

         return m_value; // NOLINT
      }
      constexpr auto borrow() & -> value_type&
      {
         detail::handle_invalid_value_result_access(is_ok());

         return m_value; // NOLINT
      }
      constexpr auto take() const&& -> value_type
      {
         detail::handle_invalid_value_result_access(is_ok());

         return std::move(m_value); // NOLINT
      }
      constexpr auto take() && -> value_type
      {
         detail::handle_invalid_value_result_access(is_ok());

         return std::move(m_value); // NOLINT
      }

      template <std::convertible_to<value_type> U>
      constexpr auto take_or(U&& other) const&& -> value_type
      {
         if (is_ok())
         {
            return std::move(m_value); // NOLINT
         }

         return std::forward<U>(other);
      }
      template <std::convertible_to<value_type> U>
      constexpr auto take_or(U&& other) && -> value_type
      {
         if (is_ok())
         {
            return std::move(m_value); // NOLINT
         }

         return std::forward<U>(other);
      }

      constexpr auto borrow_err() const& -> const error_type&
      {
         detail::handle_invalid_value_result_access(is_err());

         return m_error; // NOLINT
      }
      constexpr auto borrow_err() & -> error_type&
      {
         detail::handle_invalid_value_result_access(is_err());

         return m_error; // NOLINT
      }
      constexpr auto take_err() const&& -> error_type
      {
         detail::handle_invalid_value_result_access(is_err());

         return std::move(m_error); // NOLINT
      }
      constexpr auto take_err() && -> error_type
      {
         detail::handle_invalid_value_result_access(is_err());

         return std::move(m_error); // NOLINT
      }

      template <std::convertible_to<value_type> U>
      constexpr auto take_err_or(U&& other) const&& -> error_type
      {
         if (is_ok())
         {
            return std::move(m_value); // NOLINT
         }

         return std::forward<U>(other);
      }
      template <std::convertible_to<value_type> U>
      constexpr auto take_err_or(U&& other) && -> error_type
      {
         if (is_err())
         {
            return std::move(m_error); // NOLINT
         }

         return std::forward<U>(other);
      }

      [[nodiscard]] constexpr auto is_ok() const noexcept -> bool { return m_is_ok; }
      [[nodiscard]] constexpr auto is_err() const noexcept -> bool { return not is_ok(); }
      constexpr explicit operator bool() const noexcept { return is_ok(); }

   private:
      bool m_is_ok;

      union
      {
         value_type m_value;
         error_type m_error;
      };
   };

   template <typename ValueType, typename ErrorType>
   constexpr auto operator==(const result<ValueType, ErrorType>& lhs,
                             const result<ValueType, ErrorType>& rhs) -> bool
   {
      if (lhs.is_ok() and rhs.is_ok())
      {
         return lhs.borrow() == rhs.borrow();
      }

      if (lhs.is_err() and rhs.is_err())
      {
         return lhs.borrow_err() == rhs.borrow_err();
      }

      return false;
   }

   template <typename ValueType, typename ErrorType, std::convertible_to<ValueType> T>
   constexpr auto operator==(const result<ValueType, ErrorType>& lhs, const T& rhs) -> bool
   {
      if (lhs.is_ok())
      {
         return lhs.borrow() == rhs;
      }

      return false;
   }

   template <typename ValueType, typename ErrorType, std::convertible_to<ErrorType> T>
   constexpr auto operator==(const result<ValueType, ErrorType>& lhs, const T& rhs) -> bool
   {
      if (lhs.is_err())
      {
         return lhs.borrow_err() == rhs;
      }

      return false;
   }
} // namespace reglisse
