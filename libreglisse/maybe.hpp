/**
 * @file maybe.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Saturday, 22nd of may 2021
 * @brief Contains everything related to the maybe monad
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_MAYBE_HPP
#define LIBREGLISSE_MAYBE_HPP

#if defined(LIBREGLISSE_USE_EXCEPTIONS)
#   include <libreglisse/detail/invalid_access_exception.hpp>
#else
#   include <cassert>
#endif // defined (LIBREGLISSE_USE_EXCEPTIONS)

#include <libreglisse/operations/pipe_closure.hpp>

#include <compare>
#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

namespace reglisse::detail
{
   inline void handle_invalid_maybe_access(bool check)
   {
#if defined(LIBREGLISSE_USE_EXCEPTIONS)
      if (!check)
      {
         throw invalid_access_exception("no value stored in maybe");
      }
#else
      assert(check && "no value stored in maybe"); // NOLINT
#endif // defined(LIBREGLISSE_USE_EXCEPTIONS)
   }
} // namespace reglisse::detail

namespace reglisse
{
   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class some;

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

   static inline constexpr auto none = none_t(); ///< Shorthand for empty maybe creation

   /**
    * @brief Helper class to construct a maybe monad with value.
    */
   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class [[nodiscard]] some
   {
   public:
      using value_type = T;

   public:
      explicit constexpr some(const value_type& value) : m_value(value) {}
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

   // clang-format off
   some(const char*) -> some<std::string>;
   // clang-format on

   /**
    * @brief A monadic type that may or may not contain a value.
    */
   template <typename T>
      requires(not std::is_reference_v<T>)
   class [[nodiscard]] maybe
   {
   public:
      using value_type = T;

   public:
      constexpr maybe() noexcept {}; // NOLINT
      /**
       * @brief construct an empty monad.
       */
      constexpr maybe(none_t) noexcept {};
      /**
       * @brief construct monad from some value.
       *
       * @param val The value to take.
       */
      constexpr maybe(some<T>&& val) : m_is_none(false), m_value(std::move(val.value())) {}
      /**
       * @brief Copy construct a maybe.
       */
      constexpr maybe(const maybe& other) : m_is_none(other.is_none())
      {
         if (other.is_some())
         {
            std::construct_at(&m_value, other.m_value); // NOLINT
         }
      }
      /**
       * @brief Move construct a maybe.
       */
      constexpr maybe(maybe&& other) noexcept : m_is_none(other.is_none())
      {
         if (other.is_some())
         {
            std::construct_at(&m_value, std::move(other.m_value)); // NOLINT
         }
      }
      /**
       * @brief Destroy maybe.
       */
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
               std::construct_at(&m_value, rhs.borrow()); // NOLINT
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
               std::construct_at(&m_value, std::move(rhs.borrow())); // NOLINT
            }
         }
      }

      /**
       * @brief Borrow the value stored in the monad.
       *
       * If the monad does not hold a value, an assert will be thrown at debug time will
       * be thrown. If you wish to have runtime checking, defining the LIBREGLISSE_USE_EXCEPTIONS
       * macro before including this file will turn all assertions into exceptions.
       *
       * @returns The value stored in the monad.
       */
      constexpr auto borrow() & -> value_type&
      {
         detail::handle_invalid_maybe_access(is_some());

         return m_value; // NOLINT
      }
      /**
       * @brief Borrow the value stored in the monad.
       *
       * If the monad does not hold a value, an assert will be thrown at debug time will
       * be thrown. If you wish to have runtime checking, defining the LIBREGLISSE_USE_EXCEPTIONS
       * macro before including this file will turn all assertions into exceptions.
       *
       * @returns The value stored in the monad.
       */
      constexpr auto borrow() const& -> const value_type&
      {
         detail::handle_invalid_maybe_access(is_some());

         return m_value; // NOLINT
      }
      /**
       * @brief Take the value stored in the monad.
       *
       * This operation leaves the monad in an undefined state, it is not recommended to use it
       * after this function being called.
       *
       * If the monad does not hold a value, an assert will be thrown at debug time
       * will be thrown. If you wish to have runtime checking, defining the
       * LIBREGLISSE_USE_EXCEPTIONS macro before including this file will turn all assertions
       * into exceptions.
       *
       * @returns The value stored in the monad.
       */
      constexpr auto take() && -> value_type
      {
         detail::handle_invalid_maybe_access(is_some());

         return std::move(m_value); // NOLINT
      }
      /**
       * @brief Take the value stored in the monad.
       *
       * This operation leaves the monad in an undefined state, it is not recommended to use it
       * after this function being called.
       *
       * If the monad does not hold a value, an assert will be thrown at debug time
       * will be thrown. If you wish to have runtime checking, defining the
       * LIBREGLISSE_USE_EXCEPTIONS macro before including this file will turn all assertions
       * into exceptions.
       *
       * @returns The value stored in the monad.
       */
      constexpr auto take() const&& -> value_type
      {
         detail::handle_invalid_maybe_access(is_some());

         return std::move(m_value); // NOLINT
      }

      /**
       * @brief Take the value stored in the monad or get a default.
       *
       * This operation may leave the monad in an undefined state, it is not recommended to use it
       * after this function being called.
       *
       * @param [in] or_val The default value to use in case monad is empty
       *
       * @returns The value stored in the monad or the provided or_val.
       */
      template <std::convertible_to<value_type> U>
      constexpr auto take_or(U&& or_val) && -> value_type
      {
         if (is_some())
         {
            return std::move(m_value); // NOLINT
         }

         return static_cast<value_type>(std::forward<U>(or_val));
      }
      /**
       * @brief Take the value stored in the monad or get a default.
       *
       * This operation may leave the monad in an undefined state, it is not recommended to use it
       * after this function being called.
       *
       * @param [in] or_val The default value to use in case monad is empty
       *
       * @returns The value stored in the monad or the provided or_val.
       */
      template <std::convertible_to<value_type> U>
      constexpr auto take_or(U&& or_val) const&& -> value_type
      {
         if (is_some())
         {
            return std::move(m_value); // NOLINT
         }

         return static_cast<value_type>(std::forward<U>(or_val));
      }

      /**
       * @brief Reset the monad to it's default state
       */
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
            std::swap(borrow(), other.borrow());
         }
         else if (is_some() && other.is_none())
         {
            other.m_is_none = false;
            other.borrow() = borrow();

            m_is_none = true;
            std::destroy_at(&m_value); // NOLINT
         }
         else if (is_none() && other.is_some())
         {
            m_is_none = false;
            borrow() = other.borrow();

            other.m_is_none = true;
            std::destroy_at(&other.m_value); // NOLINT
         }
      }

      /**
       * @brief Check if the monad holds some value.
       *
       * @returns true if monad is not empty
       */
      [[nodiscard]] constexpr auto is_some() const noexcept -> bool { return not is_none(); }
      /**
       * @brief Check if the monad is empty.
       *
       * @returns true if monad is empty
       */
      [[nodiscard]] constexpr auto is_none() const noexcept -> bool { return m_is_none; }
      /**
       * @brief Check if the monad holds some value.
       *
       * @returns true if monad is not empty
       */
      [[nodiscard]] constexpr operator bool() const noexcept { return is_some(); }

   private:
      bool m_is_none = true;

      union
      {
         std::byte m_dummy{};
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
              const maybe<Second>& rhs) noexcept(noexcept(lhs.borrow() == rhs.borrow())) -> bool
   {
      if (lhs.is_some() != rhs.is_some())
      {
         return false;
      }

      if (lhs.is_none())
      {
         return true;
      }

      return lhs.borrow() == rhs.borrow();
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
   template <class Any, std::convertible_to<Any> Other>
   constexpr auto operator==(const maybe<Any>& m, const Other& value) -> bool
   {
      return m.is_some() ? m.borrow() == value : false;
   }

   template <class First, class Second>
   constexpr auto operator<=>(const maybe<First>& lhs, const maybe<Second>& rhs)
      -> std::compare_three_way_result_t<First, Second>
   {
      if (lhs.is_some() && rhs.is_some())
      {
         return lhs.borrow() <=> rhs.borrow();
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
                              const Other& value) noexcept(noexcept(m.borrow() <=> value))
      -> std::compare_three_way_result_t<Any, Other>
   {
      return m.is_some() ? m.borrow() <=> value : std::strong_ordering::less;
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
