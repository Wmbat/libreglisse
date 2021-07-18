/**
 * @file either.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Monday, 7th of june 2021
 * @brief Contains everything related to the either monad
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

   /**
    * @brief Helper class to construct a left-handed either monad
    */
   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class left
   {
   public:
      using value_type = T;

   public:
      /**
       * @brief Construct by copying a value_type
       *
       * @param [in] value The value to be stored.
       */
      explicit constexpr left(const value_type& value) : m_value(value) {}
      /**
       * @brief Construct by moving a value_type.
       *
       * @param [in] value The value to move into the class.
       */
      explicit constexpr left(value_type&& value) : m_value(std::move(value)) {}

      /**
       * @brief Borrow the value stored within the class
       *
       * @return An immutable reference to the value stored within the class.
       */
      constexpr auto borrow() const& noexcept -> const value_type& { return m_value; }
      /**
       * @brief Borrow the value stored within the class
       *
       * @return A mutable reference to the value stored within the class.
       */
      constexpr auto borrow() & noexcept -> value_type& { return m_value; }
      /**
       * @brief Take the value stored within the class. This operation leaves the class in an
       * undefined state.
       *
       * @return The value stored within the class
       */
      constexpr auto take() const&& noexcept -> const value_type { return std::move(m_value); }
      /**
       * @brief Take the value stored within the class. This operation leaves the class in an
       * undefined state.
       *
       * @return The value stored within the class
       */
      constexpr auto take() && noexcept -> value_type { return std::move(m_value); }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const left<U>& rhs) const -> bool
      {
         return borrow() == rhs.borrow();
      }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const right<U>& rhs) const -> bool
      {
         return borrow() == rhs.borrow();
      }

   private:
      value_type m_value;
   };

   /**
    * @brief Helper class to construct a right-handed either monad
    */
   template <std::movable T>
      requires(not std::is_reference_v<T>)
   class right
   {
   public:
      using value_type = T;

   public:
      /**
       * @brief Construct a right by copying a value_type
       *
       * @param [in] value The value to be stored.
       */
      explicit constexpr right(const value_type& value) : m_value(value) {}
      /**
       * @brief Construct a right by moving a value_type
       *
       * @param [in] value The value to be stored.
       */
      explicit constexpr right(value_type&& value) : m_value(std::move(value)) {}

      /**
       * @brief Borrow the value stored within the class
       *
       * @return An immutable reference to the value stored within the class.
       */
      constexpr auto borrow() const& noexcept -> const value_type& { return m_value; }
      /**
       * @brief Borrow the value stored within the class
       *
       * @return A mutable reference to the value stored within the class.
       */
      constexpr auto borrow() & noexcept -> value_type& { return m_value; }
      /**
       * @brief Take the value stored within the class. This operation leaves the class in an
       * undefined state.
       *
       * @return The value stored within the class
       */
      constexpr auto take() const&& noexcept -> const value_type { return std::move(m_value); }
      /**
       * @brief Take the value stored within the class. This operation leaves the class in an
       * undefined state.
       *
       * @return The value stored within the class
       */
      constexpr auto take() && noexcept -> value_type { return std::move(m_value); }

      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const right<U>& rhs) const -> bool
      {
         return borrow() == rhs.borrow();
      }
      template <std::equality_comparable_with<value_type> U>
      constexpr auto operator==(const left<U>& rhs) const -> bool
      {
         return borrow() == rhs.borrow();
      }

   private:
      value_type m_value;
   };

   /**
    * @brief Deduce a 'const char*' param as a std::string
    */
   left(const char*)->left<std::string>;
   /**
    * @brief Deduce a 'const char*' param as a std::string
    */
   right(const char*)->right<std::string>;

   /**
    * @brief A monadic type that contains an element either one of it's left or right side.
    */
   template <std::movable L, std::movable R>
      requires(not(std::is_reference_v<L> or std::is_reference_v<R>))
   class either
   {
   public:
      using left_type = L;  ///< Type alias for L
      using right_type = R; ///< Type alias for R

   public:
      constexpr either() = delete;
      /**
       * @brief Construct from a left either.
       *
       * @param left_val Temporary value to store
       */
      constexpr either(left<left_type>&& left_val)
      {
         std::construct_at(&m_left, std::move(left_val).take()); // NOLINT
      }
      /**
       * @brief Construct from a right either.
       *
       * @param right_val Temporary value to store
       */
      constexpr either(right<right_type>&& right_val) : m_is_left(false)
      {
         std::construct_at(&m_right, std::move(right_val).take()); // NOLINT
      }
      /**
       * @brief Copy construct an either
       */
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
      /**
       * @brief Move construct an either
       */
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
      /**
       * @brief Destruct either.
       */
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

      /**
       * @brief Borrow the value stored on the left side of the monad.
       *
       * If the monad does not hold a value on the left, an assert will be thrown at debug time will
       * be thrown. If you wish to have runtime checking, defining the LIBREGLISSE_USE_EXCEPTIONS
       * macro before including this file will turn all assertions into exceptions.
       *
       * @returns The value stored on the left side of the monad.
       */
      constexpr auto borrow_left() const& noexcept -> const left_type&
      {
         detail::handle_invalid_left_either_access(is_left());

         return m_left; // NOLINT
      }
      /**
       * @brief Borrow the value stored on the left side of the monad.
       *
       * If the monad does not hold a value on the left, an assert will be thrown at debug time will
       * be thrown. If you wish to have runtime checking, defining the
       * LIBREGLISSE_USE_EXCEPTIONS macro before including this file will turn all assertions
       * into exceptions.
       *
       * @returns The value stored on the left side of the monad.
       */
      constexpr auto borrow_left() & noexcept -> left_type&
      {
         detail::handle_invalid_left_either_access(is_left());

         return m_left; // NOLINT
      }
      /**
       * @brief Take the value stored on the left side of the monad.
       *
       * This operation leaves the monad in an undefined state, it is not recommended to use it
       * after this function being called.
       *
       * If the monad does not hold a value on the left, an assert will be thrown at debug time will
       * be thrown. If you wish to have runtime checking, defining the
       * LIBREGLISSE_USE_EXCEPTIONS macro before including this file will turn all assertions
       * into exceptions.
       *
       * @returns The value stored on the left side of the monad.
       */
      constexpr auto take_left() const&& noexcept -> const left_type
      {
         detail::handle_invalid_left_either_access(is_left());

         return std::move(m_left); // NOLINT
      }
      /**
       * @brief Take the value stored on the left side of the monad.
       *
       * This operation leaves the monad in an undefined state, it is not recommended to use it
       * after this function being called.
       *
       * If the monad does not hold a value on the left, an assert will be thrown at debug time will
       * be thrown. If you wish to have runtime checking, defining the
       * LIBREGLISSE_USE_EXCEPTIONS macro before including this file will turn all assertions
       * into exceptions.
       *
       * @returns The value stored on the left side of the monad.
       */
      constexpr auto take_left() && noexcept -> left_type
      {
         detail::handle_invalid_left_either_access(is_left());

         return std::move(m_left); // NOLINT
      }

      /**
       * @brief Borrow the value stored on the right side of the monad.
       *
       * If the monad does not hold a value on the right, an assert will be thrown at debug time
       * will be thrown. If you wish to have runtime checking, defining the
       * LIBREGLISSE_USE_EXCEPTIONS macro before including this file will turn all assertions
       * into exceptions.
       *
       * @returns The value stored on the right side of the monad.
       */
      constexpr auto borrow_right() const& noexcept -> const right_type&
      {
         detail::handle_invalid_right_either_access(is_right());

         return m_right; // NOLINT
      }
      /**
       * @brief Borrow the value stored on the right side of the monad.
       *
       * If the monad does not hold a value on the right, an assert will be thrown at debug time
       * will be thrown. If you wish to have runtime checking, defining the
       * LIBREGLISSE_USE_EXCEPTIONS macro before including this file will turn all assertions
       * into exceptions.
       *
       * @returns The value stored on the right side of the monad.
       */
      constexpr auto borrow_right() & noexcept -> right_type&
      {
         detail::handle_invalid_right_either_access(is_right());

         return m_right; // NOLINT
      }
      /**
       * @brief Take the value stored on the right side of the monad.
       *
       * This operation leaves the monad in an undefined state, it is not recommended to use it
       * after this function being called.
       *
       * If the monad does not hold a value on the right, an assert will be thrown at debug time
       * will be thrown. If you wish to have runtime checking, defining the
       * LIBREGLISSE_USE_EXCEPTIONS macro before including this file will turn all assertions
       * into exceptions.
       *
       * @returns The value stored on the right side of the monad.
       */
      constexpr auto take_right() const&& noexcept -> const right_type
      {
         detail::handle_invalid_right_either_access(is_right());

         return std::move(m_right); // NOLINT
      }
      /**
       * @brief Take the value stored on the right side of the monad.
       *
       * This operation leaves the monad in an undefined state, it is not recommended to use it
       * after this function being called.
       *
       * If the monad does not hold a value on the right, an assert will be thrown at debug time
       * will be thrown. If you wish to have runtime checking, defining the
       * LIBREGLISSE_USE_EXCEPTIONS macro before including this file will turn all assertions
       * into exceptions.
       *
       * @returns The value stored on the right side of the monad.
       */
      constexpr auto take_right() && noexcept -> right_type
      {
         detail::handle_invalid_right_either_access(is_right());

         return std::move(m_right); // NOLINT
      }

      /**
       * @brief Check if the monad is storing a value on the left.
       *
       * @returns true if it holds a value on the left
       */
      [[nodiscard]] constexpr auto is_left() const noexcept -> bool { return m_is_left; }
      /**
       * @brief Check if the monad is storing a value on the right
       *
       * @returns true if it holds a value on the right
       */
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
