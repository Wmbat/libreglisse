#pragma once

#include "monads/either.hpp"

namespace monad
{
   // clang-format off
   template <class error_, class fun_, class... args_>
      requires std::invocable<fun_, args_> 
   auto try_wrap(const fun_& fun, args_&&... args)
      -> either<error_, std::invoke_result_t<fun_, args_...>>
   {
      try
      {
         return to_right(std::invoke(fun, std::forward<args_>(args)...));
      }
      catch (const error_& e)
      {
         return to_left(e);
      }
   }
   // clang-format on
} // namespace monad
