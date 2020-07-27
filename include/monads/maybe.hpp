#pragma once

#include "monads/type_traits.hpp"

#include <cassert>
#include <compare>
#include <functional>
#include <memory>
#include <utility>

namespace monad
{
   // clang-format off
   template <class any_>
      requires(!std::is_reference_v<any_>) 
   class maybe;
   // clang-format on

   template <>
   class maybe<void>
   {
   };

   // clang-format off
   template <class any_>
      requires(!std::is_reference_v<any_>) 
   class maybe
   // clang-format on
   {
      template <class type_, class dummy_ = void>
      class storage
      {
      public:
         using value_type = type_;

      private:
         // clang-format off
         static inline constexpr bool is_nothrow_default_constructible =
            std::is_nothrow_default_constructible_v<std::array<std::byte, sizeof(value_type)>>;

         static inline constexpr bool is_nothrow_copy_value_constructible =
            std::is_nothrow_copy_constructible_v<value_type> && 
            std::is_nothrow_default_constructible_v<std::array<std::byte, sizeof(value_type)>>;
         
         static inline constexpr bool is_nothrow_move_value_constructible =
            std::is_nothrow_move_constructible_v<value_type> &&
            std::is_nothrow_default_constructible_v<std::array<std::byte, sizeof(value_type)>>;

         static inline constexpr bool is_nothrow_destructible = 
            std::is_nothrow_destructible_v<value_type> &&
            std::is_nothrow_destructible_v<std::array<std::byte, sizeof(value_type)>>;

         static inline constexpr bool is_nothrow_copy_assignable =
            is_nothrow_copy_value_constructible && 
            is_nothrow_default_constructible;

         static inline constexpr bool is_nothrow_move_assignable =
            is_nothrow_move_value_constructible && 
            is_nothrow_default_constructible;

         static inline constexpr bool is_nothrow_value_copyable =
            std::is_nothrow_copy_assignable_v<value_type> &&
            std::is_nothrow_copy_constructible_v<value_type>;

         static inline constexpr bool is_nothrow_value_movable =
            std::is_nothrow_move_assignable_v<value_type> &&
            std::is_nothrow_move_constructible_v<value_type>;
         // clang-format on

      public:
         constexpr storage() noexcept(is_nothrow_default_constructible) = default;
         constexpr storage(const value_type& value) noexcept(is_nothrow_copy_value_constructible) :
            m_is_engaged{true}
         {
            std::construct_at(pointer(), value);
         }
         constexpr storage(value_type&& value) noexcept(is_nothrow_move_value_constructible) :
            m_is_engaged{true}
         {
            std::construct_at(pointer(), std::move(value));
         }
         constexpr storage(const storage& rhs) noexcept(is_nothrow_copy_value_constructible) :
            m_is_engaged{rhs.engaged()}
         {
            if (engaged())
            {
               std::construct_at(pointer(), rhs.value());
            }
         }
         constexpr storage(storage&& rhs) noexcept(is_nothrow_move_value_constructible) :
            m_is_engaged{rhs.engaged()}
         {
            if (engaged())
            {
               std::construct_at(pointer(), std::move(rhs.value));
               rhs.m_is_engaged = false;
            }
         }
         ~storage() noexcept(is_nothrow_destructible)
         {
            if (engaged())
            {
               std::destroy_at(pointer());
            }
         }

         constexpr auto operator=(const storage& rhs) noexcept(is_nothrow_copy_assignable)
            -> storage&
         {
            if (this != &rhs)
            {
               if (engaged())
               {
                  std::destroy_at(pointer());
               }

               m_is_engaged = rhs.engaged();

               if (m_is_engaged)
               {
                  std::construct_at(pointer(), rhs.value());
               }
            }

            return *this;
         }
         constexpr auto operator=(storage&& rhs) noexcept(is_nothrow_move_assignable) -> storage&
         {
            if (this != &rhs)
            {
               if (engaged())
               {
                  std::destroy_at(pointer());
               }

               m_is_engaged = rhs.engaged();
               rhs.m_is_engaged = false;

               if (engaged())
               {
                  std::construct_at(pointer(), std::move(rhs.value()));
               }
            }

            return *this;
         }

         constexpr auto pointer() noexcept -> value_type*
         {
            return reinterpret_cast<value_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto pointer() const noexcept -> const value_type*
         {
            return reinterpret_cast<const value_type*>(m_bytes.data()); // NOLINT
         }

         constexpr auto value() & noexcept -> value_type& { return *pointer(); }
         constexpr auto value() const& noexcept(is_nothrow_value_copyable) -> const value_type&
         {
            return *pointer();
         }
         constexpr auto value() && noexcept(is_nothrow_value_movable) -> value_type&&
         {
            return std::move(*pointer());
         }
         constexpr auto value() const&& noexcept(is_nothrow_value_movable) -> const value_type&&
         {
            return std::move(*pointer());
         }

         [[nodiscard]] constexpr auto engaged() const noexcept -> bool { return m_is_engaged; }

      private:
         alignas(value_type) std::array<std::byte, sizeof(value_type)> m_bytes;
         bool m_is_engaged{false};
      };

      template <class type_>
      class storage<type_, std::enable_if_t<trivial<type_>>>
      {
      public:
         using value_type = type_;

      public:
         constexpr storage() noexcept = default;
         constexpr storage(const value_type& value) noexcept : m_is_engaged{true}
         {
            std::construct_at(pointer(), value);
         }
         constexpr storage(value_type&& value) noexcept : m_is_engaged{true}
         {
            std::construct_at(pointer(), std::move(value));
         }

         constexpr auto pointer() noexcept -> value_type*
         {
            return reinterpret_cast<value_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto pointer() const noexcept -> const value_type*
         {
            return reinterpret_cast<const value_type*>(m_bytes.data()); // NOLINT
         }

         constexpr auto value() & noexcept -> value_type& { return *pointer(); }
         constexpr auto value() const& noexcept -> const value_type& { return *pointer(); }
         constexpr auto value() && noexcept -> value_type&& { return std::move(*pointer()); }
         constexpr auto value() const&& noexcept -> const value_type&&
         {
            return std::move(*pointer());
         }

         [[nodiscard]] constexpr auto engaged() const noexcept -> bool { return m_is_engaged; }

      private:
         alignas(value_type) std::array<std::byte, sizeof(value_type)> m_bytes;
         bool m_is_engaged{false};
      };

      using storage_type = storage<any_>;

      static inline constexpr bool is_nothrow_default_constructible =
         std::is_nothrow_default_constructible_v<storage_type>;

      static inline constexpr bool is_nothrow_rvalue_constructible =
         std::is_nothrow_constructible_v<storage_type, any_&&>;

      static inline constexpr bool is_nothrow_lvalue_constructible =
         std::is_nothrow_constructible_v<storage_type, any_>;

      static inline constexpr bool is_nothrow_value_copyable =
         std::is_nothrow_copy_assignable_v<any_> && std::is_nothrow_copy_constructible_v<any_>;

      static inline constexpr bool is_nothrow_value_movable =
         std::is_nothrow_move_assignable_v<any_> && std::is_nothrow_move_constructible_v<any_>;

   public:
      using value_type = typename storage_type::value_type;

   public:
      constexpr maybe() noexcept(is_nothrow_default_constructible) = default;
      constexpr maybe(const value_type& value) noexcept(is_nothrow_rvalue_constructible) :
         m_storage{value}
      {}
      constexpr maybe(value_type&& value) noexcept(is_nothrow_lvalue_constructible) :
         m_storage{std::move(value)}
      {}
      constexpr maybe(maybe<void>) noexcept(is_nothrow_default_constructible) {}

      constexpr auto operator->() noexcept -> value_type*
      {
         assert(has_value());

         return m_storage.pointer();
      }
      constexpr auto operator->() const noexcept -> const value_type*
      {
         assert(has_value());

         return m_storage.pointer();
      }

      constexpr auto operator*() & noexcept -> value_type& { return m_storage.value(); }
      constexpr auto operator*() const& noexcept(is_nothrow_value_copyable) -> const value_type&
      {
         return m_storage.value();
      }
      constexpr auto operator*() && noexcept(is_nothrow_value_movable) -> value_type&&
      {
         return std::move(m_storage.value());
      }
      constexpr auto operator*() const&& noexcept(is_nothrow_value_movable) -> const value_type&&
      {
         return std::move(m_storage.value());
      }

      constexpr auto value() & noexcept -> value_type&
      {
         assert(has_value());

         return m_storage.value();
      }
      constexpr auto value() const& noexcept(is_nothrow_value_copyable) -> const value_type&
      {
         assert(has_value());

         return m_storage.value();
      }
      constexpr auto value() && noexcept(is_nothrow_value_movable) -> value_type&&
      {
         assert(has_value());

         return std::move(m_storage.value());
      }
      constexpr auto value() const&& noexcept(is_nothrow_value_movable) -> const value_type&&
      {
         assert(has_value());

         return std::move(m_storage.value());
      }

      constexpr auto value_or(
         std::convertible_to<value_type> auto&& default_value) const& -> value_type
      {
         return has_value()
            ? value()
            : static_cast<value_type>(std::forward<decltype(default_value)>(default_value));
      }
      constexpr auto value_or(std::convertible_to<value_type> auto&& default_value) && -> value_type
      {
         return has_value()
            ? value()
            : static_cast<value_type>(std::forward<decltype(default_value)>(default_value));
      }

      [[nodiscard]] constexpr auto has_value() const noexcept -> bool
      {
         return m_storage.engaged();
      }
      constexpr operator bool() const noexcept { return has_value(); }

      constexpr auto map(const std::invocable<value_type> auto& fun) const
      {
         using result_type = std::invoke_result_t<decltype(fun), value_type>;
         if (!has_value())
         {
            return maybe<result_type>{};
         }
         else
         {
            return maybe<result_type>(std::invoke(fun, value()));
         }
      }
      constexpr auto map(const std::invocable<value_type> auto& fun)
      {
         using result_type = std::invoke_result_t<decltype(fun), value_type>;
         if (!has_value())
         {
            return maybe<result_type>{};
         }
         else
         {
            if constexpr (std::movable<value_type>)
            {
               return maybe<result_type>(std::invoke(fun, std::move(value())));
            }
            else
            {
               return maybe<result_type>(std::invoke(fun, value()));
            }
         }
      }

   private:
      storage<value_type> m_storage{};
   };

   template <class any_>
   constexpr auto to_maybe(any_&& value) noexcept(
      std::is_nothrow_constructible_v<maybe<any_>, any_>) -> maybe<std::remove_reference_t<any_>>
   {
      return {std::forward<any_>(value)};
   }

   using none_t = maybe<void>;

   static inline constexpr auto none = none_t{}; // NOLINT

   template <class first_, std::equality_comparable_with<first_> second_>
   constexpr auto operator==(const maybe<first_>& lhs, const maybe<second_>& rhs) noexcept(
      noexcept(lhs.value() == rhs.value())) -> bool
   {
      if (lhs.has_value() != rhs.has_value())
      {
         return false;
      }
      else if (!lhs.has_value())
      {
         return true;
      }
      else
      {
         return lhs.value() == rhs.value();
      }
   }
   template <class any_>
   constexpr auto operator==(const maybe<any_>& m, none_t) noexcept
   {
      return !m.has_value();
   }
   template <class any_>
   constexpr auto operator==(const maybe<any_>& m,
      const std::equality_comparable_with<any_> auto& value) noexcept(noexcept(m.value() == value))
   {
      return m.has_value() ? m.value() == value : false;
   }

   template <class first_, std::three_way_comparable_with<first_> second_>
   constexpr auto operator<=>(const maybe<first_>& lhs, const maybe<second_>& rhs)
      -> std::compare_three_way_result_t<first_, second_>
   {
      if (lhs.has_value() && rhs.has_value())
      {
         return lhs.value() <=> rhs.has_value();
      }
      else
      {
         return lhs.has_value() <=> rhs.has_value();
      }
   }

   template <class any_>
   constexpr auto operator<=>(const maybe<any_>& m, none_t) noexcept -> std::strong_ordering
   {
      return m.has_value() <=> false;
   }

   template <class any_>
   constexpr auto operator<=>(const maybe<any_>& m,
      const std::three_way_comparable_with<any_> auto& value) noexcept(noexcept(m.value() <=>
      value)) -> std::compare_three_way_result_t<any_, decltype(value)>
   {
      return m.has_value() ? m.value() <=> value : std::strong_ordering::less;
   }
} // namespace monad
