/**
 * @file maybe.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Saturday, 22nd of may 2021
 * @brief Contains the in_place_t struct.
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_MAYBE_HPP
#define LIBREGLISSE_MAYBE_HPP

#include <deque>
#include <iostream>
#include <libreglisse/type_traits.hpp>
#include <libreglisse/utils/concepts.hpp>

#include <gsl/gsl_assert>

#include <cassert>
#include <compare>
#include <functional>
#include <memory>
#include <utility>

namespace reglisse
{
   template <typename T>
   requires(not std::is_reference_v<T>) class some;

   /**
    * @brief Represents an empty maybe
    */
   struct [[nodiscard]] none_t
   {
      constexpr auto operator==(const none_t&) const noexcept -> bool { return true; }

      template <typename Any>
      constexpr auto operator==(const some<Any>&) const noexcept -> bool
      {
         return false;
      }
   };

   /**
    * @brief
    */
   static inline constexpr auto none = none_t();

   /**
    * @brief
    *
    * @tparam T
    */
   template <typename T>
   requires(not std::is_reference_v<T>) class [[nodiscard]] some
   {
   public:
      using value_type = T;

   public:
      explicit constexpr some(value_type&& value) : m_value(std::move(value)) {}

      constexpr auto value() const& noexcept -> const value_type& { return m_value; }
      constexpr auto value() & noexcept -> value_type& { return m_value; }
      constexpr auto value() const&& noexcept -> const value_type { return std::move(m_value); }
      constexpr auto value() && noexcept -> value_type { return std::move(m_value); }

      constexpr auto operator==(const some<value_type>& rhs) const
         -> bool requires std::equality_comparable<value_type>
      {
         return value() == rhs.value();
      }

      constexpr auto operator==(none_t) const noexcept -> bool { return false; }

   private:
      value_type m_value;
   };

   /**
    * @brief
    *
    * @tparam T The type being held by the maybe monad.
    */
   template <typename T>
   requires(not std::is_reference_v<T>) class [[nodiscard]] maybe
   {
   public:
      using value_type = T;

   public:
      /**
       * @brief Create an empty monad.
       */
      constexpr maybe() noexcept {}; // NOLINT
      /**
       * @brief Create an empty maybe monad explicitly from a none_t.
       */
      constexpr maybe(none_t) noexcept {};
      /**
       * @brief Create an monad from a value by copy.
       *
       * @param val
       */
      constexpr maybe(const some<T>& val) : m_is_none(false), m_value(val.value()) {}
      /**
       * @brief Create an monad from a value by move.
       *
       * @param val
       */
      constexpr maybe(some<T>&& val) : m_is_none(false), m_value(std::move(val.value())) {}
      constexpr maybe(const maybe& other) : m_is_none(other.is_none())
      {
         if (other.is_some())
         {
            std::construct_at(&m_value, other.m_value); // NOLINT
         }
      }
      constexpr maybe(maybe&& other) noexcept : m_is_none(other.is_none())
      {
         if (other.is_some())
         {
            std::construct_at(&m_value, std::move(other.m_value)); // NOLINT
         }
      }
      constexpr ~maybe()
      {
         if (is_some())
         {
            std::destroy_at(&m_value); // NOLINT
         }
      }

      constexpr auto operator=(const maybe& rhs) -> maybe&
      {
         if (this != &rhs)
         {
            if (is_some())
            {
               std::destroy_at(&m_value); // NOLINT
            }

            m_is_none = rhs.is_none();
            rhs.m_is_none = false;

            if (is_some())
            {
               std::construct_at(&m_value, rhs.value()); // NOLINT
            }
         }
      }
      constexpr auto operator=(maybe&& rhs) noexcept -> maybe&
      {
         if (this != &rhs)
         {
            if (is_some())
            {
               std::destroy_at(&m_value); // NOLINT
            }

            m_is_none = rhs.is_none();
            rhs.m_is_none = false;

            if (is_some())
            {
               std::construct_at(&m_value, std::move(rhs.value())); // NOLINT
            }
         }
      }

      constexpr auto value() & noexcept -> value_type&
      {
         Expects(is_some());

         return m_value; // NOLINT
      }
      constexpr auto value() const& noexcept -> const value_type&
      {
         Expects(is_some());

         return m_value; // NOLINT
      }
      constexpr auto take() && noexcept -> value_type
      {
         Expects(is_some());

         return std::move(m_value); // NOLINT
      }
      constexpr auto take() const&& noexcept -> const value_type
      {
         Expects(is_some());

         return std::move(m_value); // NOLINT
      }

      template <std::convertible_to<value_type> U>
      constexpr auto take_or(U&& or_val) && -> value_type
      {
         if (is_some())
         {
            return std::move(m_value); // NOLINT
         }

         return static_cast<value_type>(std::forward<U>(or_val));
      }
      template <std::convertible_to<value_type> U>
      constexpr auto take_or(U&& or_val) const&& -> value_type
      {
         if (is_some())
         {
            return std::move(m_value); // NOLINT
         }

         return static_cast<value_type>(std::forward<U>(or_val));
      }

      constexpr void reset()
      {
         if (is_some())
         {
            std::destroy_at(&m_value); // NOLINT
            m_is_none = true;
         }
      }

      constexpr void swap(maybe& other) requires std::swappable<value_type>
      {
         if (is_some() && other.is_some())
         {
            std::swap(value(), other.value());
         }
         else if (is_some() && other.is_none())
         {
            other.m_is_none = false;
            other.value() = value();

            m_is_none = true;
            std::destroy_at(&m_value); // NOLINT
         }
         else if (is_none() && other.is_some())
         {
            m_is_none = false;
            value() = other.value();

            other.m_is_none = true;
            std::destroy_at(&other.m_value); // NOLINT
         }
      }

      [[nodiscard]] constexpr auto is_some() const noexcept -> bool { return not is_none(); }
      [[nodiscard]] constexpr auto is_none() const noexcept -> bool { return m_is_none; }
      [[nodiscard]] constexpr operator bool() const noexcept { return is_some(); }

      template <std::invocable<value_type> Fun>
      constexpr auto
      transform(Fun&& some_fun) const& -> maybe<std::invoke_result_t<Fun, value_type>>
      {
         if (is_some())
         {
            return some(std::invoke(std::forward<Fun>(some_fun), value()));
         }

         return none;
      }
      template <std::invocable<value_type> Fun>
      constexpr auto transform(Fun&& some_fun) & -> maybe<std::invoke_result_t<Fun, value_type>>
      {
         if (is_some())
         {
            return some(std::invoke(std::forward<Fun>(some_fun), value()));
         }

         return none;
      }
      template <std::invocable<value_type> Fun>
      constexpr auto
      transform(Fun&& some_fun) const&& -> maybe<std::invoke_result_t<Fun, value_type&&>>
      {
         if (is_some())
         {
            return some(std::invoke(std::forward<Fun>(some_fun), std::move(m_value))); // NOLINT
         }

         return none;
      }
      template <std::invocable<value_type> Fun>
      constexpr auto transform(Fun&& some_fun) && -> maybe<std::invoke_result_t<Fun, value_type&&>>
      {
         if (is_some())
         {
            return some(std::invoke(std::forward<Fun>(some_fun), std::move(m_value))); // NOLINT
         }

         return none;
      }

      template <std::invocable<value_type> Fun, class Other>
      constexpr auto transform_or(Fun&& some_fun, Other&& other)
         const& -> std::common_type_t<std::invoke_result_t<Fun, const value_type&>, Other>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), value());
         }

         return std::forward<Other>(other);
      }
      template <std::invocable<value_type> Fun, class Other>
      constexpr auto transform_or(
         Fun&& some_fun,
         Other&& other) & -> std::common_type_t<std::invoke_result_t<Fun, value_type&>, Other>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), value());
         }

         return std::forward<Other>(other);
      }
      template <std::invocable<value_type> Fun, class Other>
      constexpr auto transform_or(Fun&& some_fun, Other&& other)
         const&& -> std::common_type_t<std::invoke_result_t<Fun, value_type&&>, Other>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), std::move(m_value)); // NOLINT
         }

         return std::forward<Other>(other);
      }
      template <std::invocable<value_type> Fun, class Other>
      constexpr auto transform_or(
         Fun&& some_fun,
         Other&& other) && -> std::common_type_t<std::invoke_result_t<Fun, value_type&&>, Other>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), std::move(m_value)); // NOLINT
         }

         return std::forward<Other>(other);
      }

      template <std::invocable<value_type> Fun>
      constexpr auto and_then(Fun&& some_fun) const& -> std::invoke_result_t<Fun, value_type>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), value());
         }

         return none;
      }
      template <std::invocable<value_type> Fun>
      constexpr auto and_then(Fun&& some_fun) & -> std::invoke_result_t<Fun, value_type>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), value());
         }

         return none;
      }
      template <std::invocable<value_type> Fun>
      constexpr auto and_then(Fun&& some_fun) const&& -> std::invoke_result_t<Fun, value_type>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), std::move(m_value)); // NOLINT
         }

         return none;
      }
      template <std::invocable<value_type> Fun>
      constexpr auto and_then(Fun&& some_fun) && -> std::invoke_result_t<Fun, value_type>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), std::move(m_value)); // NOLINT
         }

         return none;
      }

      template <std::invocable Fun>
      constexpr auto or_else(Fun&& none_fun) const& -> maybe<value_type>
      {
         if (is_some())
         {
            return *this;
         }

         return std::invoke(std::forward<Fun>(none_fun));
      }
      template <std::invocable Fun>
      constexpr auto or_else(Fun&& none_fun) & -> maybe<value_type>
      {
         if (is_some())
         {
            return *this;
         }

         return std::invoke(std::forward<Fun>(none_fun));
      }
      template <std::invocable Fun>
      constexpr auto or_else(Fun&& none_fun) const&& -> maybe<value_type>
      {
         if (is_some())
         {
            return std::move(*this);
         }

         return std::invoke(std::forward<Fun>(none_fun));
      }
      template <std::invocable Fun>
      constexpr auto or_else(Fun&& none_fun) && -> maybe<value_type>
      {
         if (is_some())
         {
            return std::move(*this);
         }

         return std::invoke(std::forward<Fun>(none_fun));
      }

      template <std::invocable<value_type> Fun, std::invocable Def>
      requires std::convertible_to<std::invoke_result_t<Fun, value_type&&>,
                                   std::invoke_result_t<Def>> constexpr auto
      transform_or_else(Fun&& some_fun, Def&& none_fun) const& -> std::invoke_result_t<Def>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), value());
         }

         return std::invoke(std::forward<Def>(none_fun));
      }
      template <std::invocable<value_type> Fun, std::invocable Def>
      requires std::convertible_to<std::invoke_result_t<Fun, value_type&&>,
                                   std::invoke_result_t<Def>> constexpr auto
      transform_or_else(Fun&& some_fun, Def&& none_fun) & -> std::invoke_result_t<Def>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), value());
         }

         return std::invoke(std::forward<Def>(none_fun));
      }
      template <std::invocable<value_type> Fun, std::invocable Def>
      requires std::convertible_to<std::invoke_result_t<Fun, value_type&&>,
                                   std::invoke_result_t<Def>> constexpr auto
      transform_or_else(Fun&& some_fun, Def&& none_fun) const&& -> std::invoke_result_t<Def>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), std::move(value()));
         }

         return std::invoke(std::forward<Def>(none_fun));
      }
      template <std::invocable<value_type> Fun, std::invocable Def>
      requires std::convertible_to<std::invoke_result_t<Fun, value_type&&>,
                                   std::invoke_result_t<Def>> constexpr auto
      transform_or_else(Fun&& some_fun, Def&& none_fun) && -> std::invoke_result_t<Def>
      {
         if (is_some())
         {
            return std::invoke(std::forward<Fun>(some_fun), std::move(value()));
         }

         return std::invoke(std::forward<Def>(none_fun));
      }

   private:
      bool m_is_none = true;

      union
      {
         value_type m_value;
      };
   };

   // clang-format on

   /**
    * @brief
    */
   template <class First, std::equality_comparable_with<First> Second>
   constexpr auto
   operator==(const maybe<First>& lhs,
              const maybe<Second>& rhs) noexcept(noexcept(lhs.value() == rhs.value())) -> bool
   {
      if (lhs.is_some() != rhs.is_some())
      {
         return false;
      }

      if (lhs.is_none())
      {
         return true;
      }

      return lhs.value() == rhs.value();
   }

   /**
    * @brief
    */
   template <class Any>
   constexpr auto operator==(const maybe<Any>& m, none_t) noexcept -> bool
   {
      return m.is_none();
   }

   /**
    * @brief
    */
   template <class Any, class Other>
   constexpr auto operator==(const maybe<Any>& m,
                             const Other& value) noexcept(noexcept(m.value() == value)) -> bool
   {
      return m.is_some() ? m.value() == value : false;
   }

   template <class First, class Second>
   constexpr auto operator<=>(const maybe<First>& lhs, const maybe<Second>& rhs) noexcept(
      noexcept(lhs.value() <=> rhs.value())) -> std::compare_three_way_result_t<First, Second>
   {
      if (lhs.is_some() && rhs.is_some())
      {
         return lhs.value() <=> rhs.value();
      }

      return lhs.is_some() <=> rhs.is_some();
   }

   template <class Any>
   constexpr auto operator<=>(const maybe<Any>& m, none_t) noexcept -> std::strong_ordering
   {
      return m.is_some() <=> false;
   }

   template <class Any, class Other>
   constexpr auto operator<=>(const maybe<Any>& m,
                              const Other& value) noexcept(noexcept(m.value() <=> value))
      -> std::compare_three_way_result_t<Any, Other>
   {
      return m.is_some() ? m.value() <=> value : std::strong_ordering::less;
   }
} // namespace reglisse

namespace std // NOLINT
{
   template <class Any>
   constexpr void swap(reglisse::maybe<Any>& lhs, reglisse::maybe<Any>& rhs)
   {
      lhs.swap(rhs);
   }
} // namespace std

#endif // LIBREGLISSE_MAYBE_HPP
