/**
 * @file either.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Monday, 7th of june 2021
 * @brief
 * @copyright Copyright (C) 2021 wmbat.
 */

#ifndef LIBREGLISSE_EITHER_HPP
#define LIBREGLISSE_EITHER_HPP

#if defined(LIBREGLISSE_USE_EXCEPTIONS)
#   include <libreglisse/detail/invalid_access_exception.hpp>
#else
#   include <cassert>
#endif // defined (LIBREGLISSE_USE_EXCEPTIONS)

#include <algorithm>
#include <concepts>

namespace reglisse::detail
{
   inline void handle_invalid_left_either_access(bool check)
   {
#if defined(LIBREGLISSE_USE_EXCEPTIONS)
      if (!check)
      {
         throw invalid_access_exception("value stored on right side of either");
      }
#else
      assert(check && "value stored on right side of either"); // NOLINT
#endif // defined(LIBREGLISSE_USE_EXCEPTIONS)
   }

   inline void handle_invalid_right_either_access(bool check)
   {
#if defined(LIBREGLISSE_USE_EXCEPTIONS)
      if (!check)
      {
         throw invalid_access_exception("value stored on left side of either");
      }
#else
      assert(check && "value stored on left side of either");  // NOLINT
#endif // defined(LIBREGLISSE_USE_EXCEPTIONS)
   }
} // namespace reglisse::detail

namespace reglisse
{
   template <std::movable LeftType, std::movable RightType>
      requires(not(std::is_reference_v<LeftType> or std::is_reference_v<RightType>))
   class either;

   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class left;

   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class right;

   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class left
   {
   public:
      using value_type = T;

   public:
      explicit constexpr left(const value_type& value) : m_value(value) {}
      explicit constexpr left(value_type&& value) : m_value(std::move(value)) {}

      constexpr auto value() const& noexcept -> const value_type& { return m_value; }
      constexpr auto value() & noexcept -> value_type& { return m_value; }
      constexpr auto value() const&& noexcept -> const value_type { return std::move(m_value); }
      constexpr auto value() && noexcept -> value_type { return std::move(m_value); }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const left<U>& rhs) const -> bool
      {
         return value() == rhs.value();
      }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const right<U>& rhs) const -> bool
      {
         return value() == rhs.value();
      }

   private:
      value_type m_value;
   };

   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class right
   {
   public:
      using value_type = T;

   public:
      explicit constexpr right(const value_type& value) : m_value(value) {}
      explicit constexpr right(value_type&& value) : m_value(std::move(value)) {}

      constexpr auto value() const& noexcept -> const value_type& { return m_value; }
      constexpr auto value() & noexcept -> value_type& { return m_value; }
      constexpr auto value() const&& noexcept -> const value_type { return std::move(m_value); }
      constexpr auto value() && noexcept -> value_type { return std::move(m_value); }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const right<U>& rhs) const -> bool
      {
         return value() == rhs.value();
      }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const left<U>& rhs) const -> bool
      {
         return value() == rhs.value();
      }

   private:
      value_type m_value;
   };

   left(const char*)->left<std::string>;
   right(const char*)->right<std::string>;

   template <std::movable L, std::movable R>
      requires(not(std::is_reference_v<L> or std::is_reference_v<R>))
   class either
   {
   public:
      using left_type = L;
      using right_type = R;

   public:
      constexpr either() = delete;
      constexpr either(left<left_type>&& left_val)
      {
         std::construct_at(&m_left, std::move(left_val).value()); // NOLINT
      }
      constexpr either(right<right_type>&& right_val) : m_is_left(false)
      {
         std::construct_at(&m_right, std::move(right_val.value())); // NOLINT
      }
      constexpr either(const either& other) : m_is_left(other.is_left())
      {
         if (is_left())
         {
            std::construct_at(&m_left, other.borrow_left()); // NOLINT
         }
         else
         {
            std::construct_at(&m_right, other.borrow_right()); // NOLINT
         }
      }
      constexpr either(either&& other) noexcept : m_is_left(other.is_left())
      {
         if (is_left())
         {
            std::construct_at(&m_left, other.take_left()); // NOLINT
         }
         else
         {
            std::construct_at(&m_right, other.take_right()); // NOLINT
         }
      }
      constexpr ~either()
      {
         if (is_left())
         {
            std::destroy_at(&m_left); // NOLINT
         }
         else
         {
            std::destroy_at(&m_right); // NOLINT
         }
      }

      constexpr auto operator=(const either& rhs) -> either&
      {
         if (this != &rhs)
         {
            if (is_left())
            {
               std::destroy_at(&m_left); // NOLINT
            }
            else
            {
               std::destroy_at(&m_right); // NOLINT
            }

            m_is_left = rhs.m_is_left;

            if (is_left())
            {
               std::construct_at(&m_left, rhs.borrow_left()); // NOLINT
            }
            else
            {
               std::construct_at(&m_right, rhs.borrow_right()); // NOLINT
            }
         }

         return *this;
      }
      constexpr auto operator=(either&& rhs) noexcept -> either&
      {
         if (this != &rhs)
         {
            if (is_left())
            {
               std::destroy_at(&m_left); // NOLINT
            }
            else
            {
               std::destroy_at(&m_right); // NOLINT
            }

            m_is_left = rhs.m_is_left;

            if (is_left())
            {
               std::construct_at(&m_left, std::move(rhs).take_left()); // NOLINT
            }
            else
            {
               std::construct_at(&m_right, std::move(rhs).take_right()); // NOLINT
            }
         }

         return *this;
      }

      constexpr auto borrow_left() const& noexcept -> const left_type&
      {
         detail::handle_invalid_left_either_access(is_left());

         return m_left; // NOLINT
      }
      constexpr auto borrow_left() & noexcept -> left_type&
      {
         detail::handle_invalid_left_either_access(is_left());

         return m_left; // NOLINT
      }
      constexpr auto take_left() const&& noexcept -> const left_type
      {
         detail::handle_invalid_left_either_access(is_left());

         return std::move(m_left); // NOLINT
      }
      constexpr auto take_left() && noexcept -> left_type
      {
         detail::handle_invalid_left_either_access(is_left());

         return std::move(m_left); // NOLINT
      }

      constexpr auto borrow_right() const& noexcept -> const right_type&
      {
         detail::handle_invalid_right_either_access(is_right());

         return m_right; // NOLINT
      }
      constexpr auto borrow_right() & noexcept -> right_type&
      {
         detail::handle_invalid_right_either_access(is_right());

         return m_right; // NOLINT
      }
      constexpr auto take_right() const&& noexcept -> const right_type
      {
         detail::handle_invalid_right_either_access(is_right());

         return std::move(m_right); // NOLINT
      }
      constexpr auto take_right() && noexcept -> right_type
      {
         detail::handle_invalid_right_either_access(is_right());

         return std::move(m_right); // NOLINT
      }

      [[nodiscard]] constexpr auto is_left() const noexcept -> bool { return m_is_left; }
      [[nodiscard]] constexpr auto is_right() const noexcept -> bool { return !is_left(); }

   private:
      bool m_is_left = true;

      union
      {
         left_type m_left;
         right_type m_right;
      };
   };

   template <typename LeftType, typename RightType>
   constexpr auto operator==(const either<LeftType, RightType>& lhs,
                             const either<LeftType, RightType>& rhs) -> bool
   {
      if (lhs.is_left() and rhs.is_left())
      {
         return lhs.borrow_left() == rhs.borrow_left();
      }

      if (rhs.is_right() and rhs.is_right())
      {
         return lhs.borrow_right == rhs.borrow_right();
      }

      return false;
   }

   template <typename LeftType, typename RightType, std::convertible_to<LeftType> T>
   constexpr auto operator==(const either<LeftType, RightType>& lhs, const T& rhs) -> bool
   {
      return lhs.is_left() ? lhs.borrow_left() == rhs : false;
   }

   template <typename LeftType, typename RightType, std::convertible_to<RightType> T>
   constexpr auto operator==(const either<LeftType, RightType>& lhs, const T& rhs) -> bool
   {
      return lhs.is_right() ? lhs.borrow_right() == rhs : false;
   }
} // namespace reglisse

#endif // LIBREGLISSE_EITHER_HPP
