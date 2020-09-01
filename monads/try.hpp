#pragma once

#include <monads/result.hpp>

namespace monad
{
   template <class error_, class... args_>
   auto try_wrap(std::invocable<args_...> auto&& fun, args_&&... args)
      -> result<std::invoke_result_t<decltype(fun), args_...>, error_>
   {
      try
      {
         return std::invoke(std::forward<decltype(fun)>(fun), std::forward<args_>(args)...);
      }
      catch (const error_& e)
      {
         return make_error(e);
      }
   }
} // namespace monad
