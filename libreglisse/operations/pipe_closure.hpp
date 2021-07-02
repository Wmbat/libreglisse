#ifndef LIBREGLISSE_OPERATIONS_PIPE_CLOSURE_HPP
#define LIBREGLISSE_OPERATIONS_PIPE_CLOSURE_HPP

#include <libreglisse/concepts.hpp>

#include <concepts>
#include <functional>
#include <utility>

namespace reglisse
{
   namespace detail
   {
      template <typename Type>
      struct forwarding_wrapper
      {
         template <std::convertible_to<Type> U>
         constexpr forwarding_wrapper(U&& val) : value(std::forward<U>(val))
         {}

         constexpr auto get() const -> Type { return std::move(value); }

         Type value;
      };

      template <typename Type>
      constexpr auto make_forwarding_wrapper(Type&& val)
      {
         return forwarding_wrapper<Type>(std::forward<Type>(val));
      }
   } // namespace detail

   template <typename Func>
   struct pipe_closure : Func
   {
      template <typename... Types>
         requires std::constructible_from<Func, Types...>
      constexpr pipe_closure(Types&&... values) : Func(std::forward<Types>(values)...) {}
   };

   template <typename Func>
   constexpr auto make_pipe_closure(Func func)
   {
      return pipe_closure<Func>(std::move(func));
   }

   template <typename OpFunctor>
   struct operation
   {
      template <typename... Params>
      constexpr auto operator()(Params... values) const
      {
         const auto test = [](auto... wrapped_values) {
            return [=]<monad T>(T&& m) -> decltype(auto) {
               return OpFunctor()(std::forward<T>(m), wrapped_values.get()...);
            };
         }(detail::make_forwarding_wrapper(std::forward<Params>(values)...));

         return make_pipe_closure(test);
      }
   };

   template <monad T, typename Func>
   constexpr decltype(auto) operator|(T&& val, const pipe_closure<Func>& closure)
   {
      return closure(std::forward<T>(val));
   }

   template <monad T, typename Func>
   constexpr decltype(auto) operator|(T&& val, const operation<Func>& operation)
   {
      return operation()(std::forward<T>(val));
   }
} // namespace reglisse

#endif // LIBREGLISSE_OPERATIONS_PIPE_CLOSURE_HPP
