#pragma once

#include <monads/result.hpp>

namespace monad
{
   // clang-format off
   template <class error_, class fun_, class... args_>
   auto try_wrap(fun_&& fun, args_&&... args)
      -> result<std::invoke_result_t<fun_, args_...>, error_>
   requires 
      std::invocable<fun_, args_...>
   {
      try
      {
         return std::invoke(std::forward<fun_>(fun), std::forward<args_>(args)...);
      }
      catch (const error_& e)
      {
         return make_error(e);
      }
   }

   template <class error_, class fun_, class... args_>
   auto try_wrap(fun_&& fun, args_&&... args) -> maybe<error_> 
   requires 
      std::invocable<fun_, args_...> && 
      std::same_as<void, std::invoke_result_t<fun_, args_...>>
   {
      try
      {
         std::invoke(std::forward<fun_>(fun), std::forward<args_>(args)...);
      }
      catch (const error_& e)
      {
         return {e};
      }
     
      return none;
   }
   // clang-format on
} // namespace monad
