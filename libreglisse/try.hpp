#pragma once

#include <libreglisse/result.hpp>

namespace reglisse
{
   template <class ErrType, class Fun, class... Args>
      requires std::invocable<Fun, Args...>
   constexpr auto try_wrap(Fun&& fun, Args&&... args)
   {
      try
      {
         return std::invoke(std::forward<Fun>(fun), std::forward<Args>(args)...);
      }
      catch (const ErrType& e)
      {
         return err(e);
      }
   }
} // namespace reglisse
