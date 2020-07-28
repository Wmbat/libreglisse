#pragma once

#include "monads/either.hpp"

namespace monad
{
   template <class error_, class... args_>
   auto try_wrap(const std::invocable<args_...> auto& fun, args_&&... args)
      -> either<error_, std::invoke_result_t<decltype(fun), args_...>>
   {
      try
      {
         return make_right(std::invoke(fun, std::forward<args_>(args)...));
      }
      catch (const error_& e)
      {
         return make_left(e);
      }
   }
} // namespace monad
