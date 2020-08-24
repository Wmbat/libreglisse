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
   template <class any_> requires(!std::is_reference_v<any_>) 
   class maybe;
   // clang-format on

   template <>
   class maybe<void>
   {
   };

   using none_t = maybe<void>;
   static inline constexpr auto none = none_t{}; // NOLINT

   // clang-format off
   template <class any_> requires(!std::is_reference_v<any_>) 
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
         static constexpr bool is_nothrow_copy_value_constructible =
            std::is_nothrow_copy_constructible_v<value_type>;
         
         static constexpr bool is_nothrow_move_value_constructible =
            std::is_nothrow_move_constructible_v<value_type>;

         static constexpr bool is_nothrow_destructible = 
            std::is_nothrow_destructible_v<value_type>;

         static constexpr bool is_nothrow_copy_assignable =
            is_nothrow_copy_value_constructible;

         static constexpr bool is_nothrow_move_assignable =
            is_nothrow_move_value_constructible;

         static constexpr bool is_nothrow_value_copyable =
            std::is_nothrow_copy_assignable_v<value_type> &&
            std::is_nothrow_copy_constructible_v<value_type>;

         static constexpr bool is_nothrow_value_movable =
            std::is_nothrow_move_assignable_v<value_type> &&
            std::is_nothrow_move_constructible_v<value_type>;

         static constexpr bool is_nothrow_swappable =
            std::is_nothrow_move_assignable_v<value_type> &&
            std::is_nothrow_destructible_v<value_type> &&
            std::is_nothrow_swappable_v<value_type>;
         // clang-format on

      public:
         constexpr storage() noexcept = default;
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
         constexpr storage(std::in_place_t, auto&&... args) noexcept(
            std::is_nothrow_constructible_v<value_type, decltype(args)...>) :
            m_is_engaged{true}
         {
            std::construct_at(pointer(), std::forward<decltype(args)>(args)...);
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
               m_is_engaged = false;
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

         [[nodiscard]] constexpr auto engaged() const noexcept -> bool { return m_is_engaged; }

         constexpr void
         swap(storage& other) noexcept(is_nothrow_swappable) requires std::swappable<value_type>
         {
            if (engaged() && other.engaged())
            {
               std::swap(value(), other.value());
            }

            if (engaged() && !other.engaged())
            {
               std::swap(m_is_engaged, other.m_is_engaged);
               other.value() = value();

               std::destroy_at(pointer());
            }

            if (!engaged() && other.engaged())
            {
               std::swap(m_is_engaged, other.m_is_engaged);
               value() = other.value();

               std::destroy_at(other.pointer());
            }
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

      private:
         alignas(value_type) std::array<std::byte, sizeof(value_type)> m_bytes;
         bool m_is_engaged{false};
      };

      template <class type_>
      class storage<type_, std::enable_if_t<trivial<type_>>>
      {
      public:
         using value_type = type_;

         constexpr storage() noexcept = default;
         constexpr storage(const value_type& value) noexcept : m_is_engaged{true}
         {
            std::construct_at(pointer(), value);
         }
         constexpr storage(value_type&& value) noexcept : m_is_engaged{true}
         {
            std::construct_at(pointer(), std::move(value));
         }
         constexpr storage(std::in_place_t, auto&&... args) noexcept : m_is_engaged{true}
         {
            std::construct_at(pointer(), std::forward<decltype(args)>(args)...);
         }

         [[nodiscard]] constexpr auto engaged() const noexcept -> bool { return m_is_engaged; }

         constexpr void swap(storage& other) noexcept requires std::swappable<value_type>
         {
            if (engaged() && other.engaged())
            {
               std::swap(value(), other.value());
            }

            if (engaged() && !other.engaged())
            {
               std::swap(m_is_engaged, other.m_is_engaged);
               other.value() = value();
            }

            if (!engaged() && other.engaged())
            {
               std::swap(m_is_engaged, other.m_is_engaged);
               value() = other.value();
            }
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

      private:
         alignas(value_type) std::array<std::byte, sizeof(value_type)> m_bytes;
         bool m_is_engaged{false};
      };

      using storage_type = storage<any_>;

      static constexpr bool is_nothrow_rvalue_constructible =
         std::is_nothrow_constructible_v<storage_type, any_&&>;

      static constexpr bool is_nothrow_lvalue_constructible =
         std::is_nothrow_constructible_v<storage_type, any_>;

      static constexpr bool is_nothrow_value_copyable =
         std::is_nothrow_copy_assignable_v<any_> && std::is_nothrow_copy_constructible_v<any_>;

      static constexpr bool is_nothrow_value_movable =
         std::is_nothrow_move_assignable_v<any_> && std::is_nothrow_move_constructible_v<any_>;

      static constexpr bool is_nothrow_swappable =
         std::is_nothrow_move_constructible_v<any_> && std::is_nothrow_swappable_v<any_>;

   public:
      using value_type = typename storage_type::value_type;

      constexpr maybe() noexcept = default;
      constexpr maybe(none_t) noexcept {}
      constexpr maybe(const value_type& value) noexcept(is_nothrow_lvalue_constructible) :
         m_storage{value}
      {}
      constexpr maybe(value_type&& value) noexcept(is_nothrow_rvalue_constructible) :
         m_storage{std::move(value)}
      {}
      constexpr maybe(std::in_place_t u, auto&&... args) noexcept(
         std::is_nothrow_constructible_v<value_type, decltype(args)...>) requires std::
         constructible_from<value_type, decltype(args)...> :
         m_storage{u, std::forward<decltype(args)>(args)...}
      {}

      /**
       * Access the stored value
       */
      constexpr auto operator->() noexcept -> value_type*
      {
         assert(has_value());

         return m_storage.pointer();
      }
      /**
       * Access the stored value
       */
      constexpr auto operator->() const noexcept -> const value_type*
      {
         assert(has_value());

         return m_storage.pointer();
      }

      /**
       * Return the stored value
       */
      constexpr auto operator*() & noexcept -> value_type& { return m_storage.value(); }
      /**
       * Return the stored value
       */
      constexpr auto operator*() const& noexcept(is_nothrow_value_copyable) -> const value_type&
      {
         return m_storage.value();
      }
      /**
       * Return the stored value
       */
      constexpr auto operator*() && noexcept(is_nothrow_value_movable) -> value_type&&
      {
         return std::move(m_storage.value());
      }
      /**
       * Return the stored value
       */
      constexpr auto operator*() const&& noexcept(is_nothrow_value_movable) -> const value_type&&
      {
         return std::move(m_storage.value());
      }

      /**
       * Check if a value is present
       */
      [[nodiscard]] constexpr auto has_value() const noexcept -> bool
      {
         return m_storage.engaged();
      }
      constexpr operator bool() const noexcept { return has_value(); }

      /**
       * Return the stored value
       */
      constexpr auto value() & noexcept -> value_type&
      {
         assert(has_value());

         return m_storage.value();
      }
      /**
       * Return the stored value
       */
      constexpr auto value() const& noexcept(is_nothrow_value_copyable) -> const value_type&
      {
         assert(has_value());

         return m_storage.value();
      }
      /**
       * Return the stored value
       */
      constexpr auto value() && noexcept(is_nothrow_value_movable) -> value_type&&
      {
         assert(has_value());

         return std::move(m_storage.value());
      }
      /**
       * Return the stored value
       */
      constexpr auto value() const&& noexcept(is_nothrow_value_movable) -> const value_type&&
      {
         assert(has_value());

         return std::move(m_storage.value());
      }

      /**
       * Take the value out of the optional into another optional,
       * leaving it empty
       */
      constexpr auto take() -> maybe
      {
         maybe ret = std::move(*this);
         reset();
         return ret;
      }

      /**
       * Return the stored value or a specified value
       */
      constexpr auto
      value_or(std::convertible_to<value_type> auto&& default_value) const& -> value_type
      {
         return has_value()
            ? value()
            : static_cast<value_type>(std::forward<decltype(default_value)>(default_value));
      }
      /**
       * Return the stored value or a specified value
       */
      constexpr auto value_or(std::convertible_to<value_type> auto&& default_value) && -> value_type
      {
         return has_value()
            ? std::move(value())
            : static_cast<value_type>(std::forward<decltype(default_value)>(default_value));
      }

      constexpr void swap(maybe& other) noexcept(noexcept(m_storage.swap(other.m_storage)))
      {
         m_storage.swap(other.m_storage);
      }

      /**
       * Destroy the stored value if it exists and set itselfs to being empty
       */
      constexpr void reset() noexcept(std::is_nothrow_destructible_v<value_type>)
      {
         if (has_value())
         {
            std::destroy_at(m_storage.pointer());
            m_storage.has_value = false;
         }
      }

      /**
       * Carries out some operation on the stored object if there is one
       */
      constexpr auto map(std::invocable<value_type> auto&& fun) const&
      {
         using result_type = std::invoke_result_t<decltype(fun), value_type>;

         return !has_value()
            ? maybe<result_type>{}
            : maybe<result_type>{std::invoke(std::forward<decltype(fun)>(fun), value())};
      }
      /**
       * Carries out some operation on the stored object if there is one
       */
      constexpr auto map(std::invocable<value_type> auto&& fun) &
      {
         using result_type = std::invoke_result_t<decltype(fun), value_type>;

         return !has_value()
            ? maybe<result_type>{}
            : maybe<result_type>{std::invoke(std::forward<decltype(fun)>(fun), value())};
      }
      /**
       * Carries out some operation on the stored object if there is one
       */
      constexpr auto map(std::invocable<value_type> auto&& fun) const&&
      {
         using result_type = std::invoke_result_t<decltype(fun), value_type>;

         return !has_value()
            ? maybe<result_type>{}
            : maybe<result_type>{std::invoke(std::forward<decltype(fun)>(fun), std::move(value()))};
      }
      /**
       * Carries out some operation on the stored object if there is one
       */
      constexpr auto map(std::invocable<value_type> auto&& fun) &&
      {
         using result_type = std::invoke_result_t<decltype(fun), value_type>;

         return !has_value()
            ? maybe<result_type>{}
            : maybe<result_type>{std::invoke(std::forward<decltype(fun)>(fun), std::move(value()))};
      }

      /**
       * Carries out an operation on the stored object if there is one, or returns
       * a default value
       */
      constexpr auto
      map_or(std::invocable<value_type> auto&& fun,
             std::convertible_to<std::invoke_result_t<decltype(fun), value_type>> auto&& other)
         const& -> decltype(other)
      {
         return has_value() ? std::invoke(std::forward<decltype(fun)>(fun), value())
                            : std::forward<decltype(other)>(other);
      }
      /**
       * Carries out an operation on the stored object if there is one, or returns
       * a default value
       */
      constexpr auto
      map_or(std::invocable<value_type> auto&& fun,
             std::convertible_to<std::invoke_result_t<decltype(fun), value_type>> auto&&
                other) & -> decltype(other)
      {
         return has_value() ? std::invoke(std::forward<decltype(fun)>(fun), value())
                            : std::forward<decltype(other)>(other);
      }
      /**
       * Carries out an operation on the stored object if there is one, or returns
       * a default value
       */
      constexpr auto
      map_or(std::invocable<value_type> auto&& fun,
             std::convertible_to<std::invoke_result_t<decltype(fun), value_type>> auto&& other)
         const&& -> decltype(other)
      {
         return has_value() ? std::invoke(std::forward<decltype(fun)>(fun), std::move(value()))
                            : std::forward<decltype(other)>(other);
      }
      /**
       * Carries out an operation on the stored object if there is one, or returns
       * a default value
       */
      constexpr auto
      map_or(std::invocable<value_type> auto&& fun,
             std::convertible_to<std::invoke_result_t<decltype(fun), value_type>> auto&&
                other) && -> decltype(other)
      {
         return has_value() ? std::invoke(std::forward<decltype(fun)>(fun), std::move(value()))
                            : std::forward<decltype(other)>(other);
      }

      /**
       * Carries out some operation that returns a monad::maybe on the stored object
       * if there is one
       */
      constexpr auto and_then(std::invocable<value_type> auto&& fun) const&
      {
         using result_type = typename std::invoke_result_t<decltype(fun), value_type>::value_type;

         return !has_value() ? maybe<result_type>{}
                             : std::invoke(std::forward<decltype(fun)>(fun), value());
      }
      /**
       * Carries out some operation that returns a monad::maybe on the stored object
       * if there is one
       */
      constexpr auto and_then(std::invocable<value_type> auto&& fun) &
      {
         using result_type = typename std::invoke_result_t<decltype(fun), value_type>::value_type;

         return !has_value() ? maybe<result_type>{}
                             : std::invoke(std::forward<decltype(fun)>(fun), value());
      }
      /**
       * Carries out some operation that returns a monad::maybe on the stored object
       * if there is one
       */
      constexpr auto and_then(std::invocable<value_type> auto&& fun) const&&
      {
         using result_type = typename std::invoke_result_t<decltype(fun), value_type>::value_type;

         return !has_value() ? maybe<result_type>{}
                             : std::invoke(std::forward<decltype(fun)>(fun), std::move(value()));
      }
      /**
       * Carries out some operation that returns a monad::maybe on the stored object
       * if there is one
       */
      constexpr auto and_then(std::invocable<value_type> auto&& fun) &&
      {
         using result_type = typename std::invoke_result_t<decltype(fun), value_type>::value_type;

         return !has_value() ? maybe<result_type>{}
                             : std::invoke(std::forward<decltype(fun)>(fun), std::move(value()));
      }

      /**
       * Carries out an operation if there is no value stored
       */
      constexpr auto or_else(std::invocable auto&& fun) const& -> maybe<value_type>
      {
         if (has_value())
         {
            return *this;
         }
         else
         {
            std::invoke(std::forward<decltype(fun)>(fun));

            return none;
         }
      }
      /**
       * Carries out an operation if there is no value stored
       */
      constexpr auto or_else(std::invocable auto&& fun) & -> maybe<value_type>
      {
         if (has_value())
         {
            return *this;
         }
         else
         {
            std::invoke(std::forward<decltype(fun)>(fun));

            return none;
         }
      }
      /**
       * Carries out an operation if there is no value stored
       */
      constexpr auto or_else(std::invocable auto&& fun) const&& -> maybe<value_type>
      {
         if (has_value())
         {
            return std::move(*this);
         }
         else
         {
            std::invoke(std::forward<decltype(fun)>(fun));

            return none;
         }
      }
      /**
       * Carries out an operation if there is no value stored
       */
      constexpr auto or_else(std::invocable auto&& fun) && -> maybe<value_type>
      {
         if (has_value())
         {
            return std::move(*this);
         }
         else
         {
            std::invoke(std::forward<decltype(fun)>(fun));

            return none;
         }
      }

      /**
       * Carries out an operation on the stored object if there is one, or return
       * a the result of a given function
       */
      constexpr auto map_or_else(std::invocable<value_type> auto&& fun, std::invocable auto&& def)
         const& -> std::invoke_result_t<decltype(def)> requires std::convertible_to<
            std::invoke_result_t<decltype(fun), value_type>, std::invoke_result_t<decltype(def)>>
      {
         return has_value() ? std::invoke(std::forward<decltype(fun)>, value())
                            : std::invoke(std::forward<decltype(def)>);
      }
      /**
       * Carries out an operation on the stored object if there is one, or return
       * a the result of a given function
       */
      constexpr auto
      map_or_else(std::invocable<value_type> auto&& fun,
                  std::invocable auto&& def) & -> std::invoke_result_t<decltype(def)> requires std::
         convertible_to<std::invoke_result_t<decltype(fun), value_type>,
                        std::invoke_result_t<decltype(def)>>
      {
         return has_value() ? std::invoke(std::forward<decltype(fun)>, value())
                            : std::invoke(std::forward<decltype(def)>);
      }
      /**
       * Carries out an operation on the stored object if there is one, or return
       * a the result of a given function
       */
      constexpr auto map_or_else(std::invocable<value_type> auto&& fun, std::invocable auto&& def)
         const&& -> std::invoke_result_t<decltype(def)> requires std::convertible_to<
            std::invoke_result_t<decltype(fun), value_type>, std::invoke_result_t<decltype(def)>>
      {
         return has_value() ? std::invoke(std::forward<decltype(fun)>, std::move(value()))
                            : std::invoke(std::forward<decltype(def)>);
      }
      /**
       * Carries out an operation on the stored object if there is one, or return
       * a the result of a given function
       */
      constexpr auto map_or_else(std::invocable<value_type> auto&& fun,
                                 std::invocable auto&& def) && -> std::
         invoke_result_t<decltype(def)> requires std::convertible_to<
            std::invoke_result_t<decltype(fun), value_type>, std::invoke_result_t<decltype(def)>>
      {
         return has_value() ? std::invoke(std::forward<decltype(fun)>, std::move(value()))
                            : std::invoke(std::forward<decltype(def)>);
      }

   private:
      storage<value_type> m_storage{};
   };

   template <class any_>
   constexpr auto make_maybe(any_&& value) -> maybe<std::decay_t<any_>>
   {
      return maybe<std::decay_t<any_>>{std::forward<any_>(value)};
   }

   template <class first_, std::equality_comparable_with<first_> second_>
   constexpr auto
   operator==(const maybe<first_>& lhs,
              const maybe<second_>& rhs) noexcept(noexcept(lhs.value() == rhs.value())) -> bool
   {
      if (lhs.has_value() != rhs.has_value())
      {
         return false;
      }

      if (!lhs.has_value())
      {
         return true;
      }

      return lhs.value() == rhs.value();
   }

   template <class any_>
   constexpr auto operator==(const maybe<any_>& m, none_t) noexcept
   {
      return !m.has_value();
   }

   template <class any_>
   constexpr auto operator==(
      const maybe<any_>& m,
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

      return lhs.has_value() <=> rhs.has_value();
   }

   template <class any_>
   constexpr auto operator<=>(const maybe<any_>& m, none_t) noexcept -> std::strong_ordering
   {
      return m.has_value() <=> false;
   }

   template <class any_>
   constexpr auto operator<=>(const maybe<any_>& m,
                              const std::three_way_comparable_with<any_> auto&
                                 value) noexcept(noexcept(m.value() <=> value))
      -> std::compare_three_way_result_t<any_, decltype(value)>
   {
      return m.has_value() ? m.value() <=> value : std::strong_ordering::less;
   }
} // namespace monad

namespace std // NOLINT
{
   template <class any_>
   constexpr void swap(monad::maybe<any_>& lhs,
                       monad::maybe<any_>& rhs) noexcept(noexcept(lhs.swap(rhs)))
   {
      lhs.swap(rhs);
   }
} // namespace std
