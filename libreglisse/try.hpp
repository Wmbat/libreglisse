#pragma once

#include <libreglisse/result.hpp>

namespace monad
{
   // clang-format off
   template <class Err, class Fun, class... Args>
   requires 
      std::invocable<Fun, Args...>
   constexpr auto try_wrap(Fun&& fun, Args&&... args) 
      -> result<std::invoke_result_t<Fun, Args...>, Err>
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
   // clang-format on
} // namespace monad
