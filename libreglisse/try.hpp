/**
 * @file try.hpp
 * @author wmbat wmbat@protonmail.com
 * @date Monday, 7th of june 2021
 * @brief Utility functions to facilitate holding exceptions in monadic types.
 * @copyright Copyright (C) 2021 wmbat.
 */

#pragma once

#include <libreglisse/result.hpp>

namespace reglisse::inline v0
{
   /**
    * @brief Wrap a call that throws an exception and returns it as a result.
    *
    * Calls a function that may throw an exception and transform the return type into a result
    * monad that contains either the return type of the function, or the exception type.
    *
    * @param fun The function to call.
    * @param args The arguments of the function.
    */
   template <class ErrType, class Fun, class... Args>
      requires std::invocable<Fun, Args...>
   constexpr auto try_wrap(Fun&& fun, Args&&... args)
      -> result<std::invoke_result_t<Fun, Args...>, ErrType>
   {
      try
      {
         return ok(std::invoke(std::forward<Fun>(fun), std::forward<Args>(args)...));
      }
      catch (const ErrType& e)
      {
         return err(e);
      }
   }
} // namespace reglisse::v0
