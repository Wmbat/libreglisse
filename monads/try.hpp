#pragma once

#include <monads/result.hpp>

namespace monad
{
   // clang-format off
   template <class Err, class Fun, class... Args>
   auto try_wrap(Fun&& fun, Args&&... args)
      -> result<std::invoke_result_t<Fun, Args...>, Err>
   requires 
      std::invocable<Fun, Args...>
   {
      try
      {
         return std::invoke(std::forward<Fun>(fun), std::forward<Args>(args)...);
      }
      catch (const Err& e)
      {
         return err(e);
      }
   }

   template <class Err, class Fun, class... Args>
   auto try_wrap(Fun&& fun, Args&&... args) -> maybe<Err> 
   requires 
      std::invocable<Fun, Args...> && 
      std::same_as<void, std::invoke_result_t<Fun, Args...>>
   {
      try
      {
         std::invoke(std::forward<Fun>(fun), std::forward<Args>(args)...);

         return none;
      }
      catch (const Err& e)
      {
         return {e};
      }
   }
   // clang-format on
} // namespace monad
