#pragma once

#include <monads/either.hpp>

namespace monad
{
   namespace detail
   {
      // clang-format off
      template <typename Fun, typename Value>
      concept result_function = requires
      {
          std::invocable<Fun, Value>;
      
          { std::invoke_result_t<Fun, Value>::value_type };
          { std::invoke_result_t<Fun, Value>::error_type };
      };
      
      template<typename Fun, typename Value, typename Error>
      concept ensure_result_error = requires
      {
          result_function<Fun, Value>;
      
          { typename std::invoke_result_t<Fun, Value>::error_type{} } -> std::same_as<Error>;
      };

      template<typename Fun, typename Value, typename Error>
      concept ensure_result_value = requires
      {
          result_function<Fun, Error>;
      
          { typename std::invoke_result_t<Fun, Error>::value_type{} } -> std::same_as<Value>;
      };
      // clang-format on

   } // namespace detail

   template <class Any>
   struct error_t
   {
      Any val;
   };

   template <class Err>
   constexpr auto err(Err&& err) -> error_t<std::decay_t<Err>>
   {
      return {std::forward<Err>(err)};
   }

   struct error_in_place_t
   {
      constexpr error_in_place_t() noexcept = default;
   };

   static constexpr error_in_place_t error_in_place;

   // static constexpr error_t error;

   // clang-format off
   template <class Value, typename Error> 
      requires (!(std::is_reference_v<Value> || std::is_reference_v<Error>))
   class [[nodiscard("a result should never be discarded")]] result
   // clang-format on
   {
      template <class First, class Second, class Dummy = void>
      class storage
      {
      public:
         using value_type = First;
         using error_type = Second;

      private:
         using storage_type =
            std::array<std::byte, detail::max(sizeof(value_type), sizeof(error_type))>;

         // clang-format off
         
         static inline constexpr bool is_nothrow_move_error_constructible = 
            std::is_nothrow_move_assignable_v<error_type> &&
            std::is_nothrow_move_constructible_v<error_type> &&
            std::is_nothrow_default_constructible_v<storage_type>;

         static inline constexpr bool is_nothrow_move_constructible =
            std::is_nothrow_move_constructible_v<value_type> && 
            std::is_nothrow_move_constructible_v<error_type>;

         static inline constexpr bool is_nothrow_copy_constructible =
            std::is_nothrow_copy_constructible_v<value_type> && 
            std::is_nothrow_copy_constructible_v<error_type>;

         static inline constexpr bool is_nothrow_destructible = 
            std::is_nothrow_destructible_v<value_type> &&
            std::is_nothrow_destructible_v<error_type> &&
            std::is_nothrow_destructible_v<storage_type>;

         static inline constexpr bool is_nothrow_copy_assignable =
            is_nothrow_copy_constructible &&
            is_nothrow_destructible;

         static inline constexpr bool is_nothrow_move_assignable =
            is_nothrow_destructible;

         static inline constexpr bool is_nothrow_value_value_copyable =
            std::is_nothrow_copy_assignable_v<value_type> &&
            std::is_nothrow_copy_constructible_v<value_type>;

         static inline constexpr bool is_nothrow_value_value_movable =
            std::is_nothrow_move_assignable_v<value_type> &&
            std::is_nothrow_move_constructible_v<value_type>;

         static inline constexpr bool is_nothrow_error_value_copyable =
            std::is_nothrow_copy_assignable_v<error_type> &&
            std::is_nothrow_copy_constructible_v<error_type>;

         static inline constexpr bool is_nothrow_error_value_movable =
            std::is_nothrow_move_assignable_v<error_type> &&
            std::is_nothrow_move_constructible_v<error_type>;
         // clang-format on

      public:
         constexpr storage() noexcept(std::is_nothrow_default_constructible_v<error_type>) :
            m_is_error{true}
         {
            std::construct_at(std::addressof(error()), error_type{});
         };
         constexpr storage(const value_type& val) noexcept(
            std::is_nothrow_copy_assignable_v<value_type>)
         {
            std::construct_at(std::addressof(value()), val);
         }
         constexpr storage(value_type&& val) noexcept(std::is_nothrow_move_assignable_v<value_type>)
         {
            std::construct_at(std::addressof(value()), std::move(val));
         }
         constexpr storage(in_place_t, auto&&... args) noexcept(
            std::is_nothrow_constructible_v<value_type, decltype(args)...>)
         {
            std::construct_at(std::addressof(value()), std::forward<decltype(args)>(args)...);
         }
         constexpr storage(const error_t<error_type>& err) noexcept(
            std::is_nothrow_copy_assignable_v<error_type>) :
            m_is_error{true}
         {
            std::construct_at(std::addressof(error()), err.val);
         }
         constexpr storage(error_t<error_type>&& err) noexcept(
            is_nothrow_move_error_constructible) :
            m_is_error{true}
         {
            std::construct_at(std::addressof(error()), std::move(err.val));
         }
         constexpr storage(error_in_place_t, auto&&... args) noexcept(
            std::is_nothrow_constructible_v<error_type, decltype(args)...>) :
            m_is_error{true}
         {
            std::construct_at(std::addressof(error()), std::forward<decltype(args)>(args)...);
         }
         constexpr storage(const storage& rhs) noexcept(is_nothrow_copy_constructible) :
            m_is_error{rhs.m_is_error}
         {
            if (is_value())
            {
               std::construct_at(std::addressof(value()), rhs.value());
            }
            else
            {
               std::construct_at(std::addressof(error()()), rhs.error());
            }
         }
         constexpr storage(storage&& rhs) noexcept(is_nothrow_move_constructible) :
            m_is_error{rhs.m_is_error}
         {
            if (is_value())
            {
               std::construct_at(std::addressof(value()), std::move(rhs.value()));
            }
            else
            {
               std::construct_at(std::addressof(error()), std::move(rhs.error()));
            }

            rhs.m_is_error = false;
         }
         ~storage() noexcept(is_nothrow_destructible)
         {
            if (is_value())
            {
               std::destroy_at(std::addressof(value()));
            }
            else
            {
               std::destroy_at(std::addressof(error()));
            }
         }

         constexpr auto operator=(const storage& rhs) -> storage& // NOLINT
         {
            if (this != &rhs)
            {
               if (is_value())
               {
                  std::destroy_at(std::addressof(value()));
               }
               else
               {
                  std::destroy_at(std::addressof(error()));
               }

               m_is_error = rhs.m_is_error;

               if (is_value())
               {
                  std::construct_at(std::addressof(value()), rhs.value());
               }
               else
               {
                  std::construct_at(std::addressof(error()), rhs.error());
               }
            }

            return *this;
         }
         constexpr auto operator=(storage&& rhs) noexcept(is_nothrow_move_assignable) -> storage&
         {
            if (this != &rhs)
            {
               if (is_value())
               {
                  std::destroy_at(std::addressof(value()));
               }
               else
               {
                  std::destroy_at(std::addressof(error()));
               }

               m_is_error = rhs.m_is_error;
               rhs.m_is_error = false;

               if (is_value())
               {
                  std::construct_at(std::addressof(value()), rhs.value());
               }
               else
               {
                  std::construct_at(std::addressof(error()), rhs.error());
               }
            }

            return *this;
         }

         [[nodiscard]] constexpr auto is_value() const noexcept -> bool { return !m_is_error; };

         constexpr auto value() & noexcept -> value_type& { return *v_pointer(); }
         constexpr auto value() const& noexcept(is_nothrow_value_value_copyable)
            -> const value_type&
         {
            return *v_pointer();
         }
         constexpr auto value() && noexcept(is_nothrow_value_value_movable) -> value_type&&
         {
            return std::move(*v_pointer());
         }
         constexpr auto value() const&& noexcept(is_nothrow_value_value_movable)
            -> const value_type&&
         {
            return std::move(*v_pointer());
         }

         constexpr auto error() & noexcept -> error_type& { return *e_pointer(); }
         constexpr auto error() const& noexcept(is_nothrow_error_value_copyable)
            -> const error_type&
         {
            return *e_pointer();
         }
         constexpr auto error() && noexcept(is_nothrow_error_value_movable) -> error_type&&
         {
            return std::move(*e_pointer());
         }
         constexpr auto error() const&& noexcept(is_nothrow_error_value_movable)
            -> const error_type&&
         {
            return std::move(*e_pointer());
         }

      private:
         constexpr auto v_pointer() noexcept -> value_type*
         {
            return reinterpret_cast<value_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto v_pointer() const noexcept -> const value_type*
         {
            return reinterpret_cast<const value_type*>(m_bytes.data()); // NOLINT
         }

         constexpr auto e_pointer() noexcept -> error_type*
         {
            return reinterpret_cast<error_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto e_pointer() const noexcept -> const error_type*
         {
            return reinterpret_cast<const error_type*>(m_bytes.data()); // NOLINT
         }

      private:
         alignas(detail::max(alignof(value_type), alignof(error_type))) storage_type m_bytes{};
         bool m_is_error{false};
      };

      template <class First, class Second>
      class storage<First, Second, std::enable_if_t<trivial<First> && trivial<Second>>>
      {
      public:
         using value_type = First;
         using error_type = Second;

      private:
         using storage_type =
            std::array<std::byte, detail::max(sizeof(value_type), sizeof(error_type))>;

      public:
         constexpr storage() noexcept { std::construct_at(std::addressof(value()), value_type{}); };
         constexpr storage(const value_type& val) noexcept
         {
            std::construct_at(std::addressof(value()), val);
         }
         constexpr storage(value_type&& val) noexcept
         {
            std::construct_at(std::addressof(value()), std::move(val));
         }
         constexpr storage(in_place_t, auto&&... args) noexcept
         {
            std::construct_at(std::addressof(value()), std::forward<decltype(args)>(args)...);
         }
         constexpr storage(const error_t<error_type>& err) noexcept : m_is_error{true}
         {
            std::construct_at(std::addressof(error()), err.val);
         }
         constexpr storage(error_t<error_type>&& err) noexcept : m_is_error{true}
         {
            std::construct_at(std::addressof(error()), std::move(err.val));
         }
         constexpr storage(error_in_place_t, auto&&... args) noexcept : m_is_error{true}
         {
            std::construct_at(std::addressof(error()), std::forward<decltype(args)>(args)...);
         }

         constexpr auto value() & noexcept -> value_type& { return *v_pointer(); }
         constexpr auto value() const& noexcept -> const value_type& { return *v_pointer(); }
         constexpr auto value() && noexcept -> value_type&& { return std::move(*v_pointer()); }
         constexpr auto value() const&& noexcept -> const value_type&&
         {
            return std::move(*v_pointer());
         }

         constexpr auto error() & noexcept -> error_type& { return *e_pointer(); }
         constexpr auto error() const& noexcept -> const error_type& { return *e_pointer(); }
         constexpr auto error() && noexcept -> error_type&& { return std::move(*e_pointer()); }
         constexpr auto error() const&& noexcept -> const error_type&&
         {
            return std::move(*e_pointer());
         }

         [[nodiscard]] constexpr auto is_value() const noexcept -> bool { return !m_is_error; };

      private:
         constexpr auto v_pointer() noexcept -> value_type*
         {
            return reinterpret_cast<value_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto v_pointer() const noexcept -> const value_type*
         {
            return reinterpret_cast<const value_type*>(m_bytes.data()); // NOLINT
         }

         constexpr auto e_pointer() noexcept -> error_type*
         {
            return reinterpret_cast<error_type*>(m_bytes.data()); // NOLINT
         }
         constexpr auto e_pointer() const noexcept -> const error_type*
         {
            return reinterpret_cast<const error_type*>(m_bytes.data()); // NOLINT
         }

      private:
         alignas(detail::max(alignof(value_type), alignof(error_type))) storage_type m_bytes{};
         bool m_is_error{false};
      };

   public:
      using value_type = Value;
      using error_type = Error;

   private:
      using storage_type = storage<value_type, error_type>;

      static constexpr bool copyable = std::copyable<value_type> && std::copyable<error_type>;
      static constexpr bool movable = std::movable<value_type> && std::movable<error_type>;

      template <class value_fun, class error_fun>
      using join_result = std::common_type_t<std::invoke_result_t<value_fun, value_type>,
                                             std::invoke_result_t<error_fun, error_type>>;

   public:
      // clang-format off
      /**
       * Construct a result by copying a value
       */
      constexpr result(const value_type& val) 
         noexcept(std::is_nothrow_constructible_v<storage_type, value_type>) :
         m_storage{val}
      {}
      /**
       * Construct a result by moving a value
       */
      constexpr result(value_type && val) 
         noexcept(std::is_nothrow_constructible_v<storage_type, value_type&&>) :
         m_storage{std::move(val)}
      {}
      /**
       * Construct a result by constructing a value in place using a variadic set of
       * arguments
       */
      template<class... Args>
      requires std::constructible_from<value_type, Args...>
      constexpr result(in_place_t, Args&&... args) 
         noexcept(std::is_nothrow_constructible_v<storage_type, Args...>) :
         m_storage{in_place, std::forward<Args>(args)...}
      {}
      constexpr result(const error_t<error_type>& err) 
         noexcept(std::is_nothrow_constructible_v<storage_type, error_t<error_type>>) :
         m_storage{err}
      {}
      constexpr result(error_t<error_type> && err) 
         noexcept(std::is_nothrow_constructible_v<storage_type, error_t<error_type>&&>) :
         m_storage{std::move(err)}
      {}
      template<class... Args>
      requires std::constructible_from<error_type, Args...>
      constexpr result(error_in_place_t, Args&&... args) 
         noexcept(std::is_nothrow_constructible_v<storage_type, Args...>) : 
         m_storage{error_in_place, std::forward<Args>(args)...}
      {}

      /**
       * Check if the result has a value or an error
       */
      [[nodiscard]] constexpr auto is_value() const -> bool { return m_storage.is_value(); }
      /**
       * Check if the result has a value or an error
       */
      constexpr operator bool() const { return is_value(); }

      constexpr auto value() const& -> maybe<value_type>
      {
         return is_value() ? make_maybe(m_storage.value()) : none;
      }
      constexpr auto value() & -> maybe<value_type>
      {
         return is_value() ? make_maybe(m_storage.value()) : none;
      }
      constexpr auto value() const&& -> maybe<value_type>
      {
         return is_value() ? make_maybe(std::move(m_storage.value())) : none;
      }
      constexpr auto value() && -> maybe<value_type>
      {
         return is_value() ? make_maybe(std::move(m_storage.value())) : none;
      }

      /**
       * Return a maybe monad containing a copy of the error. If the result does not
       * contain an error, the maybe monad will be empty
       */
      constexpr auto error() const& -> maybe<error_type>
      {
         return is_value() ? none : make_maybe(m_storage.error());
      }
      /**
       * Return a maybe monad containing a copy of the error. If the result
       * does not contain an error, the maybe monad will be empty
       */
      constexpr auto error() & -> maybe<error_type>
      {
         return is_value() ? none : make_maybe(m_storage.error());
      }
      /**
       * Move the error contained within the result into a maybe instance. If the result
       * does not contain an error, the maybe instance will be empty.
       */
      constexpr auto error() const&& -> maybe<error_type>
      {
         return is_value() ? none : make_maybe(std::move(m_storage.error()));
      }
      /**
       * Move the error contained within the result into a maybe instance. If the result
       * does not contain an error, the maybe instance will be empty.
       */
      constexpr auto error() && -> maybe<error_type>
      {
         return is_value() ? none : make_maybe(std::move(m_storage.error()));
      }

      /**
       * Perform an operation on the value stored within the result
       */
      template<std::invocable<value_type> Fun> 
      constexpr auto map(Fun&& fun) const& 
         -> result<std::invoke_result_t<Fun, value_type>, error_type>
      {
         if (is_value())
         {
            return {std::invoke(std::forward<decltype(fun)>(fun), m_storage.value())};
         }
         else
         {
            return err(m_storage.error());
         }
      }
      /**
       * Perform an operation on the value stored within the result
       */
      template<std::invocable<value_type> Fun> 
      constexpr auto map(Fun&& fun) &
         -> result<std::invoke_result_t<Fun, value_type>, error_type>
      {
         if (is_value())
         {
            return {std::invoke(std::forward<decltype(fun)>(fun), m_storage.value())};
         }
         else
         {
            return err(m_storage.error());
         }
      }
      /**
       * Perform an operation on the value stored within the result
       */
      template<std::invocable<value_type> Fun> 
      constexpr auto map(Fun&& fun) const&& 
         -> result<std::invoke_result_t<Fun, value_type&&>, error_type>
      {
         if (is_value())
         {
            return std::invoke(std::forward<Fun>(fun), std::move(m_storage.value()));
         }
         else
         {
            return err(std::move(m_storage.error()));
         }
      }
      /**
       * Perform an operation on the value stored within the result
       */
      template<std::invocable<value_type> Fun> 
      constexpr auto map(Fun&& fun) &&
         -> result<std::invoke_result_t<Fun, value_type&&>, error_type>
      {
         if (is_value())
         {
            return std::invoke(std::forward<Fun>(fun), std::move(m_storage.value()));
         }
         else
         {
            return err(std::move(m_storage.error()));
         }
      }

      /**
       * Perform an operation on the error stored within the result
       */
      template<std::invocable<error_type> Fun> 
      constexpr auto map_error(Fun&& fun) const& 
         -> result<value_type, std::invoke_result_t<Fun, error_type>>
      {
         if (is_value())
         {
            return m_storage.value();
         }
         else
         {
            return err(std::invoke(std::forward<Fun>(fun), m_storage.error()));
         }
      }
      /**
       * Perform an operation on the error stored within the result
       */
      template<std::invocable<error_type> Fun> 
      constexpr auto map_error(Fun&& fun) &
         -> result<value_type, std::invoke_result_t<Fun, error_type>>
      {
         if (is_value())
         {
            return m_storage.value();
         }
         else
         {
            return err(std::invoke(std::forward<decltype(fun)>(fun), m_storage.error()));
         }
      }
      /**
       * Perform an operation on the error stored within the result
       */
      template<std::invocable<error_type> Fun> 
      constexpr auto map_error(Fun&& fun) const&&
         -> result<value_type, std::invoke_result_t<Fun, error_type&&>>
      {
         if (is_value())
         {
            return std::move(m_storage.value());
         }
         else
         {
            return err(std::invoke(std::forward<Fun>(fun), std::move(m_storage.error())));
         }
      }
      /**
       * Perform an operation on the error stored within the result
       */
      template<std::invocable<error_type> Fun> 
      constexpr auto map_error(Fun&& fun) && 
         -> result<value_type, std::invoke_result_t<Fun, error_type&&>>
      {
         if (is_value())
         {
            return std::move(m_storage.value());
         }
         else
         {
            return err(std::invoke(std::forward<Fun>(fun), std::move(m_storage.error())));
         }
      }

      /**
       * Applies a function that return a monad::result to the value. If the monad::result
       * does not hold a value, the function will not be applied
       */
      template<detail::ensure_result_error<value_type, error_type> Fun> 
      constexpr auto and_then(Fun&& fun) const& -> std::invoke_result_t<Fun, value_type>
      {
         if (is_value())
         {
            return std::invoke(std::forward<Fun>(fun), m_storage.value());
         }
         else
         {
            return err(m_storage.error());
         }
      }
      /**
       * Applies a function that return a monad::result to the value. If the monad::result
       * does not hold a value, the function will not be applied
       */
      template<detail::ensure_result_error<value_type, error_type> Fun> 
      constexpr auto and_then(Fun&& fun) & -> std::invoke_result_t<Fun, value_type>
      {
         if (is_value())
         {
            return std::invoke(std::forward<Fun>(fun), m_storage.value());
         }
         else
         {
            return err(m_storage.error());
         }
      }
      /**
       * Applies a function that return a monad::result to the value. If the monad::result
       * does not hold a value, the function will not be applied
       */
      template<detail::ensure_result_error<value_type, error_type> Fun> 
      constexpr auto and_then(Fun&& fun) const&& -> std::invoke_result_t<Fun, value_type>
      {
         if (is_value())
         {
            return std::invoke(std::forward<Fun>(fun), std::move(m_storage.value()));
         }
         else
         {
            return err(std::move(m_storage.error()));
         }
      }
      /**
       * Applies a function that return a monad::result to the value. If the monad::result
       * does not hold a value, the function will not be applied
       */
      template<detail::ensure_result_error<value_type, error_type> Fun> 
      constexpr auto and_then(Fun&& fun) && -> std::invoke_result_t<Fun, value_type>
      {
         if (is_value())
         {
            return std::invoke(std::forward<Fun>(fun), std::move(m_storage.value()));
         }
         else
         {
            return err(std::move(m_storage.error()));
         }
      }

      /**
       * Applies a function that return a monad::result to the error. If the monad::result
       * does not hold an error, the function will not be applied
       */
      template<detail::ensure_result_value<value_type, error_type> Fun> 
      constexpr auto or_else(Fun&& fun) const& -> std::invoke_result_t<Fun, error_type>
      {
         if (is_value())
         {
            return m_storage.value();
         }
         else
         {
            return std::invoke(std::forward<Fun>(fun), m_storage.error());
         }
      }
      /**
       * Applies a function that return a monad::result to the error. If the monad::result
       * does not hold an error, the function will not be applied
       */
      template<detail::ensure_result_value<value_type, error_type> Fun> 
      constexpr auto or_else(Fun&& fun) & -> std::invoke_result_t<Fun, error_type>
      {
         if (is_value())
         {
            return m_storage.value();
         }
         else
         {
            return std::invoke(std::forward<Fun>(fun), m_storage.error());
         }
      }
      /**
       * Applies a function that return a monad::result to the error. If the monad::result
       * does not hold an error, the function will not be applied
       */
      template<detail::ensure_result_value<value_type, error_type> Fun> 
      constexpr auto or_else(Fun&& fun) const&& -> std::invoke_result_t<Fun, error_type>
      {
         if (is_value())
         {
            return std::move(m_storage.value());
         }
         else
         {
            return std::invoke(std::forward<Fun>(fun), std::move(m_storage.error()));
         }
      }
      /**
       * Applies a function that return a monad::result to the error. If the monad::result
       * does not hold an error, the function will not be applied
       */
      template<detail::ensure_result_value<value_type, error_type> Fun> 
      constexpr auto or_else(Fun&& fun) && -> std::invoke_result_t<Fun, error_type>
      {
         if (is_value())
         {
            return std::move(m_storage.value());
         }
         else
         {
            return std::invoke(std::forward<Fun>(fun), std::move(m_storage.error()));
         }
      }

      template <class inner_value_ = value_type, class inner_error_ = error_type>
      constexpr auto join() const& -> std::common_type_t<inner_value_, inner_error_>
      {
         return is_value() ? m_storage.value() : m_storage.error();
      }
      template <class inner_value_ = value_type, class inner_error_ = error_type>
      constexpr auto join() & -> std::common_type_t<inner_value_, inner_error_>
      {
         return is_value() ? m_storage.value() : m_storage.error();
      }

      template <class inner_value_ = value_type, class inner_error_ = error_type>
      constexpr auto join() const&& -> std::common_type_t<inner_value_, inner_error_>
      {
         return is_value() ? std::move(m_storage.value()) : std::move(m_storage.error());
      }
      template <class inner_value_ = value_type, class inner_error_ = error_type>
      constexpr auto join() && -> std::common_type_t<inner_value_, inner_error_>
      {
         return is_value() ? std::move(m_storage.value()) : std::move(m_storage.error());
      }

      template<std::invocable<value_type> ValueFun, std::invocable<error_type> ErrorFun>
      constexpr auto join(ValueFun&& value_fun, ErrorFun&& error_fun) const& 
         -> join_result<ValueFun, ErrorFun>
      {
         return is_value() ? std::invoke(std::forward<ValueFun>(value_fun), m_storage.value())
                           : std::invoke(std::forward<ErrorFun>(error_fun), m_storage.error());
      }
      template<std::invocable<value_type> ValueFun, std::invocable<error_type> ErrorFun>
      constexpr auto join(ValueFun&& value_fun, ErrorFun&& error_fun) &
         -> join_result<ValueFun, ErrorFun>
      {
         return is_value() ? std::invoke(std::forward<ValueFun>(value_fun), m_storage.value())
                           : std::invoke(std::forward<ErrorFun>(error_fun), m_storage.error());
      }
      template<std::invocable<value_type> ValueFun, std::invocable<error_type> ErrorFun>
      constexpr auto join(ValueFun&& value_fun, ErrorFun&& error_fun) const&&
         -> join_result<ValueFun, ErrorFun>
      {
         return is_value() 
            ? std::invoke(std::forward<ValueFun>(value_fun), std::move(m_storage.value()))
            : std::invoke(std::forward<ErrorFun>(error_fun), std::move(m_storage.error()));
      }

      template<std::invocable<value_type> ValueFun, std::invocable<error_type> ErrorFun>
      constexpr auto join(ValueFun&& value_fun, ErrorFun&& error_fun) &&
         -> join_result<ValueFun, ErrorFun>
      {
         return is_value() 
            ? std::invoke(std::forward<ValueFun>(value_fun), std::move(m_storage.value()))
            : std::invoke(std::forward<ErrorFun>(error_fun), std::move(m_storage.error()));
      }

   private:
      storage<value_type, error_type> m_storage{};
   };
} // namespace monad
