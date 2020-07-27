#pragma once

#include "monads/maybe.hpp"

namespace monad
{
   namespace detail
   {
      consteval auto max(size_t size) noexcept -> size_t { return size; }
      consteval auto max(size_t first, size_t second, std::unsigned_integral auto... rest) noexcept
         -> size_t
      {
         return first >= second ? max(first, rest...) : max(second, rest...);
      }
   } // namespace detail

   template <class any_>
   struct left_t
   {
      any_ value;
   };

   template <class any_>
   struct right_t
   {
      any_ value;
   };

   template <class any_>
   constexpr auto to_left(any_&& value) -> left_t<std::remove_reference_t<any_>>
   {
      return {.val = std::forward<std::remove_reference_t<any_>>(value)};
   }

   template <class any_>
   constexpr auto to_right(any_&& value) -> right_t<std::remove_reference_t<any_>>
   {
      return {.val = std::forward<std::remove_reference_t<any_>>(value)};
   }

   // clang-format off
   template <class left_, class right_> 
      requires (!(std::is_reference_v<left_> || std::is_reference_v<right_>))
   class either
   // clang-format on
   {
      template <class first_, class second_, class dummy_ = void>
      class storage
      {
      public:
         using left_type = first_;
         using right_type = second_;

      private:
         using storage_type =
            std::array<std::byte, detail::max(sizeof(left_type), sizeof(right_type))>;

         // clang-format off
         static inline constexpr bool is_nothrow_default_constructible =
            std::is_nothrow_default_constructible_v<left_type> && 
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_copy_left_constructible =
            std::is_nothrow_copy_assignable_v<left_type> &&
            std::is_nothrow_copy_constructible_v<left_type> && 
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_move_left_constructible =
            std::is_nothrow_move_assignable_v<left_type> &&
            std::is_nothrow_move_constructible_v<left_type> &&
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_copy_right_constructible =
            std::is_nothrow_copy_assignable_v<right_type> &&
            std::is_nothrow_copy_constructible_v<right_type> && 
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_move_right_constructible = 
            std::is_nothrow_move_assignable_v<right_type> &&
            std::is_nothrow_move_constructible_v<right_type> &&
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_copy_constructible =
            is_nothrow_copy_left_constructible && 
            is_nothrow_copy_right_constructible;

         static inline constexpr bool is_nothrow_move_constructible =
            is_nothrow_move_left_constructible && 
            is_nothrow_move_right_constructible;

         static inline constexpr bool is_nothrow_destructible = 
            std::is_nothrow_destructible_v<left_type> &&
            std::is_nothrow_destructible_v<right_type> &&
            std::is_nothrow_destructible_v<storage_type>;

         static inline constexpr bool is_nothrow_copy_assignable =
            is_nothrow_copy_constructible &&
            is_nothrow_destructible;

         static inline constexpr bool is_nothrow_move_assignable =
            is_nothrow_move_constructible &&
            is_nothrow_destructible;
         // clang-format on

      public:
         constexpr storage() noexcept(is_nothrow_default_constructible)
         {
            std::construct_at(std::addressof(left()), left_type{});
         };
         constexpr storage(const left_t<left_type>& l) noexcept(is_nothrow_copy_left_constructible)
         {
            std::construct_at(std::addressof(left()), l.value);
         }
         constexpr storage(left_t<left_type>&& l) noexcept(is_nothrow_move_left_constructible)
         {
            std::construct_at(std::addressof(left()), std::move(l.value));
         }
         constexpr storage(const right_t<right_type>& r) noexcept(
            is_nothrow_copy_right_constructible) :
            m_is_right{true}
         {
            std::construct_at(std::addressof(right()), r.value);
         }
         constexpr storage(right_t<right_type>&& r) noexcept(is_nothrow_move_right_constructible) :
            m_is_right{true}
         {
            std::construct_at(std::addressof(right()), r.value);
         }
         constexpr storage(const storage& rhs) noexcept(is_nothrow_copy_constructible) :
            m_is_right{rhs.is_right()}
         {
            if (!is_right())
            {
               std::construct_at(std::addressof(left()), rhs.left());
            }
            else
            {
               std::construct_at(std::addressof(right()()), rhs.right());
            }
         }
         constexpr storage(storage&& rhs) noexcept(is_nothrow_move_constructible) :
            m_is_right{rhs.is_right()}
         {
            if (!is_right())
            {
               std::construct_at(std::addressof(left()), std::move(rhs.left()));
            }
            else
            {
               std::construct_at(std::addressof(right()), std::move(rhs.right()));
            }
         }
         ~storage() noexcept(is_nothrow_destructible)
         {
            if (!is_right())
            {
               std::destroy_at(std::addressof(left()));
            }
            else
            {
               std::destroy_at(std::addressof(right()));
            }
         }

         constexpr auto operator=(const storage& rhs) noexcept(is_nothrow_copy_assignable)
            -> storage&
         {
            if (this != &rhs)
            {
               if (!is_right())
               {
                  std::destroy_at(std::addressof(left()));
               }
               else
               {
                  std::destroy_at(std::addressof(right()));
               }

               m_is_right = rhs.is_right();

               if (!is_right())
               {
                  std::construct_at(std::addressof(left()), rhs.left());
               }
               else
               {
                  std::construct_at(std::addressof(right()), rhs.right());
               }
            }

            return *this;
         }
         constexpr auto operator=(storage&& rhs) noexcept(is_nothrow_move_assignable) -> storage&
         {
            if (this != &rhs)
            {
               if (!is_right())
               {
                  std::destroy_at(std::addressof(left()));
               }
               else
               {
                  std::destroy_at(std::addressof(right()));
               }

               m_is_right = rhs.is_right();

               if (!is_right())
               {
                  std::construct_at(std::addressof(left()), rhs.left());
               }
               else
               {
                  std::construct_at(std::addressof(right()), rhs.right());
               }
            }

            return *this;
         }

         constexpr auto left() & noexcept -> left_type& { return *l_pointer(); }
         constexpr auto left() const& noexcept -> const left_type& { return *l_pointer(); }
         constexpr auto left() && noexcept -> left_type&& { return std::move(*l_pointer()); }
         constexpr auto left() const&& noexcept -> const left_type&&
         {
            return std::move(*l_pointer());
         }

         constexpr auto right() & noexcept -> right_type& { return *r_pointer(); }
         constexpr auto right() const& noexcept -> const right_type& { return *r_pointer(); }
         constexpr auto right() && noexcept -> right_type&& { return std::move(*r_pointer()); }
         constexpr auto right() const&& noexcept -> const right_type&&
         {
            return std::move(*r_pointer());
         }

         [[nodiscard]] constexpr auto is_right() const noexcept -> bool { return m_is_right; };

      private:
         constexpr auto l_pointer() noexcept -> left_type*
         {
            return reinterpret_cast<left_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto l_pointer() const noexcept -> const left_type*
         {
            return reinterpret_cast<left_type*>(m_bytes.data()); // NOLINT
         }

         constexpr auto r_pointer() noexcept -> right_type*
         {
            return reinterpret_cast<right_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto r_pointer() const noexcept -> const right_type*
         {
            return reinterpret_cast<right_type*>(m_bytes.data()); // NOLINT
         }

      private:
         alignas(detail::max(alignof(left_type), alignof(right_type))) storage_type m_bytes{};
         bool m_is_right{false};
      };

      template <class first_, class second_>
      class storage<first_, second_, std::enable_if_t<trivial<first_> && trivial<second_>>>
      {
      public:
         using left_type = first_;
         using right_type = second_;

      private:
         using storage_type =
            std::array<std::byte, detail::max(sizeof(left_type), sizeof(right_type))>;

      public:
         constexpr storage() noexcept { std::construct_at(std::addressof(left()), left_type{}); };
         constexpr storage(const left_t<left_type>& l) noexcept
         {
            std::construct_at(std::addressof(left()), l.value);
         }
         constexpr storage(left_t<left_type>&& l) noexcept
         {
            std::construct_at(std::addressof(left()), std::move(l.value));
         }
         constexpr storage(const right_t<right_type>& r) noexcept : m_is_right{true}
         {
            std::construct_at(std::addressof(right()), r.value);
         }
         constexpr storage(right_t<right_type>&& r) noexcept : m_is_right{true}
         {
            std::construct_at(std::addressof(right()), std::move(r.value));
         }

         constexpr auto left() & noexcept -> left_type& { return *l_pointer(); }
         constexpr auto left() const& noexcept -> const left_type& { return *l_pointer(); }
         constexpr auto left() && noexcept -> left_type&& { return std::move(*l_pointer()); }
         constexpr auto left() const&& noexcept -> const left_type&&
         {
            return std::move(*l_pointer());
         }

         constexpr auto right() & noexcept -> right_type& { return *r_pointer(); }
         constexpr auto right() const& noexcept -> const right_type& { return *r_pointer(); }
         constexpr auto right() && noexcept -> right_type&& { return std::move(*r_pointer()); }
         constexpr auto right() const&& noexcept -> const right_type&&
         {
            return std::move(*r_pointer());
         }

         [[nodiscard]] constexpr auto is_right() const noexcept -> bool { return m_is_right; };

      private:
         constexpr auto l_pointer() noexcept -> left_type*
         {
            return reinterpret_cast<left_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto l_pointer() const noexcept -> const left_type*
         {
            return reinterpret_cast<left_type*>(m_bytes.data()); // NOLINT
         }

         constexpr auto r_pointer() noexcept -> right_type*
         {
            return reinterpret_cast<right_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto r_pointer() const noexcept -> const right_type*
         {
            return reinterpret_cast<right_type*>(m_bytes.data()); // NOLINT
         }

      private:
         alignas(detail::max(alignof(left_type), alignof(right_type))) storage_type m_bytes{};
         bool m_is_right{false};
      };

   public:
      using left_type = left_;
      using right_type = right_;

   private:
      template <class any_>
      using left_map_result = std::invoke_result_t<any_, left_type>;

      template <class fun_>
      using left_map_either = either<left_map_result<fun_>, right_type>;

      template <class any_>
      using right_map_result = std::invoke_result_t<any_, right_type>;

      template <class fun_>
      using right_map_either = either<right_type, right_map_result<fun_>>;

   public:
      constexpr either(const left_t<left_type>& l) : m_storage{l} {}
      constexpr either(left_t<left_type>&& l) : m_storage{std::move(l)} {}
      constexpr either(const right_t<right_type>& r) : m_storage{r} {}
      constexpr either(right_t<right_type>&& r) : m_storage{std::move(r)} {}

      [[nodiscard]] constexpr auto is_left() const -> bool { return !m_storage.is_right(); }
      constexpr operator bool() const { return is_left(); }

      constexpr auto left() const -> maybe<left_type>
      {
         if (is_left())
         {
            return to_maybe(left_type{m_storage.left()});
         }
         else
         {
            return none;
         }
      }
      constexpr auto left() -> maybe<left_type>
      {
         if (is_left())
         {
            if constexpr (std::movable<left_type>)
            {
               return to_maybe(left_type{std::move(m_storage.left())});
            }
            else
            {
               return to_maybe(left_type{m_storage.left()});
            }
         }
         else
         {
            return none;
         }
      }

      constexpr auto right() const -> maybe<right_type>
      {
         if (is_left())
         {
            return none;
         }
         else
         {
            return to_maybe(right_type{m_storage.right()});
         }
      }
      constexpr auto right() -> maybe<right_type>
      {
         if (is_left())
         {
            return none;
         }
         else
         {
            if constexpr (std::movable<right_type>)
            {
               return to_maybe(right_type{std::move(m_storage.right())});
            }
            else
            {
               return to_maybe(right_type{m_storage.right()});
            }
         }
      }

   private:
      storage<left_type, right_type> m_storage{};

   public:
      constexpr auto left_map(const std::invocable<left_type> auto& fun) const
         -> left_map_either<decltype(fun)>
      {
         return is_left() ? to_left(std::invoke(fun, m_storage.left()))
                          : to_right(m_storage.right());
      }

      constexpr auto left_map(const std::invocable<left_type> auto& fun)
         -> left_map_either<decltype(fun)>
      {
         if (is_left())
         {
            if constexpr (std::movable<left_type>)
            {
               return to_left(std::invoke(fun, std::move(m_storage.left())));
            }
            else
            {
               return to_left(std::invoke(fun, m_storage.left()));
            }
         }
         else
         {
            if constexpr (std::movable<right_type>)
            {
               return to_right(std::move(m_storage.right()));
            }
            else
            {
               return to_right(m_storage.right());
            }
         }
      }

      constexpr auto right_map(const std::invocable<right_type> auto& fun) const
         -> right_map_either<decltype(fun)>
      {
         return is_left() ? to_left(m_storage.left())
                          : to_right(std::invoke(fun, m_storage.right()));
      }

      constexpr auto right_map(const std::invocable<right_type> auto& fun)
         -> right_map_either<decltype(fun)>
      {
         if (is_left())
         {
            if constexpr (std::movable<left_type>)
            {
               return to_left(std::move(m_storage.left()));
            }
            else
            {
               return to_left(m_storage.left());
            }
         }
         else
         {
            if constexpr (std::movable<right_type>)
            {
               return to_right(std::invoke(fun, std::move(m_storage.right())));
            }
            else
            {
               return to_value(std::invoke(fun, m_storage.right()));
            }
         }
      }

      template <std::copyable inner_error_ = left_type, std::copyable inner_value_ = right_type>
      constexpr auto join() const -> std::common_type_t<inner_error_, inner_value_>
      {
         return is_left ? m_storage.error() : m_storage.value();
      }

      template <std::movable inner_error_ = left_type, std::movable inner_value_ = left_type>
      constexpr auto join() -> std::common_type_t<inner_error_, inner_value_>
      {
         if (is_left())
         {
            if constexpr (std::movable<left_type>)
            {
               return std::move(m_storage.left());
            }
            else
            {
               return m_storage.left();
            }
         }
         else
         {
            if constexpr (std::movable<left_type>)
            {
               return std::move(m_storage.right());
            }
            else
            {
               return m_storage.right();
            }
         }
      }

      constexpr auto join(const std::invocable<left_type> auto& left_fun,
         const std::invocable<right_type> auto& right_fun) const
         -> std::common_type_t<decltype(left_fun(m_storage.left())),
            decltype(right_fun(m_storage.right()))>
      //-> decltype(is_left() ? left_fun(m_storage.error()) : right_fun(m_storage.value()))
      {
         return is_left() ? std::invoke(left_fun, m_storage.error())
                          : std::invoke(right_fun, m_storage.value());
      }

      // clang-format off
      constexpr auto join(const std::invocable<left_type> auto& left_fun,
         const std::invocable<right_type> auto& right_fun)
         -> decltype(is_left() ? 
               left_fun(std::move(m_storage.left())) : 
               right_fun(std::move(m_storage.right()))) 
         requires std::movable<left_type> && std::movable<right_type>
      {
         return is_left() ? std::invoke(left_fun, std::move(m_storage.error()))
                          : std::invoke(right_fun, std::move(m_storage.value()));
      }
      // clang-format on
   };
} // namespace monad
