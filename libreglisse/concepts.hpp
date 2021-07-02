#ifndef LIBREGLISSE_CONCEPTS_HPP
#define LIBREGLISSE_CONCEPTS_HPP

#include <concepts>
#include <utility>

#define ERROR_TYPE_HELPER(T) typename std::remove_reference_t<T>::error_type

namespace reglisse
{
   // clang-format off

   namespace detail
   {
      template <typename Type>
      concept rvalue_maybe_access = requires(std::decay_t<Type> t)
      {
         { std::move(t).take() } -> std::same_as<typename std::decay_t<Type>::value_type>;  
      };

      template <typename Type>
      concept lvalue_maybe_access = requires(std::decay_t<Type> t)
      {
         { t.borrow() } -> std::same_as<typename std::decay_t<Type>::value_type>;  
      };

      template <typename Type>
      concept rvalue_result_access = requires(std::decay_t<Type> t)
      {
         { std::move(t).take() } -> std::same_as<typename std::decay_t<Type>::value_type>;
         { std::move(t).take_err() } -> std::same_as<typename std::decay_t<Type>::error_type>;
      };

      template <typename Type>
      concept lvalue_result_access = requires(std::decay_t<Type> t)
      {
         { t.borrow() } -> std::same_as<typename std::decay_t<Type>::value_type>;
         { t.borrow_err() } -> std::same_as<typename std::decay_t<Type>::error_type>;
      };

      template <typename Type>
      concept rvalue_either_access = requires(std::decay_t<Type> t)
      {
         { std::move(t).take_left() } -> std::same_as<typename std::decay_t<Type>::left_type>;
         { std::move(t).take_right() } -> std::same_as<typename std::decay_t<Type>::right_type>;
      };

      template <typename Type>
      concept lvalue_either_access = requires(std::decay_t<Type> t)
      {
         { t.borrow_left() } -> std::same_as<typename std::decay_t<Type>::left_type>;
         { t.borrow_right() } -> std::same_as<typename std::decay_t<Type>::right_type>;
      };
   } // namespace detail

   template <typename Type>
   concept maybe_monad = requires(Type type)
   {
      typename std::decay_t<Type>::value_type;

      { type.is_some() } -> std::same_as<bool>;
      { type.is_none() } -> std::same_as<bool>;
   } and (detail::rvalue_maybe_access<Type> or detail::lvalue_maybe_access<Type>);

   template <typename Type>
   concept result_monad = requires(Type type)
   {
      typename std::decay_t<Type>::value_type;
      ERROR_TYPE_HELPER(Type);

      { type.is_ok() } -> std::same_as<bool>;
      { type.is_err() } -> std::same_as<bool>;

      { std::move(type).take() }  -> std::same_as<typename std::decay_t<Type>::value_type>;
      { std::move(type).take_err() } -> std::same_as<ERROR_TYPE_HELPER(Type)>;
   } and (detail::rvalue_result_access<Type> or detail::lvalue_result_access<Type>);

   template <typename Type>
   concept either_monad = requires(Type type)
   {
      typename std::decay_t<Type>::left_type;
      typename std::decay_t<Type>::right_type;

      { type.is_left() } -> std::same_as<bool>;
      { type.is_right() } -> std::same_as<bool>;
   } and (detail::rvalue_either_access<Type> or detail::lvalue_either_access<Type>);
   
   template <typename Type>
   concept monad = maybe_monad<Type> or result_monad<Type> or either_monad<Type>;

   // clang-format on
} // namespace reglisse

#endif // LIBREGLISSE_CONCEPTS_HPP
