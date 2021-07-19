# What is libreglisse?

`libreglisse` is a library of monadic types implemented using C++20. It currently contains the `maybe`, `either` and `result` monads.
The library also provides a set of extensible operations that can be applied on the monadic types through the pipe
operator (`operator|`).

# Table of Content
* [Requirements](#requirements)
* [Basic Classes](#basic-classes)
* [Operations](#operations)
* [Extend the API](#extend-the-api)

# Requirements

* A C++20 capable compiler
* [build2](https://build2.org/) for build & packaging system

# Basic Classes

The library provides three monadic types: `maybe`, `result` and `either`. They are quite small in size and provide a
bare API for storing & accessing the data store within the monadic types.

### Maybe

`maybe` is a monadic type that either holds a value or is empty. It provides functions to query the state of the monad
with `is_some()` and `is_none()`. 

The monad can be constructed empty using the `none` value, or using the `some` class as follows:
```
maybe<int> m_empty = none;
maybe<int> m_full = some(1);
```

You can borrow the value stored within the monad when it is an lvalue with the function `borrow()`
You can take the value stored within the monad when it is an rvalue with the function `take()` or `take_or()`

If you attemp to **borrow** or **take** the value stored when the monad is empty, an `abort()` will be called. 

### Result

`result` is a monadic type that hold either a value or an error. It provides functions to query the state of the monad
with `is_ok()` and `is_err()`

The monad can be constructed using the `ok` and `err` classes as follows:
```
either<int, std::string> m_ok = ok(1);
either<int, std::string> m_err = err("hello");
```

You can borrow the value stored within the monad when it is an lvalue with the function `borrow()` and the error using
`borrow_err()`
You can take the value stored within the monad when it is an rvalue with the function `take()` and the error using
`take_err()`

If you attemp to **borrow** or **take** the value stored when the monad holds an error, an `abort()` will be called. The
inverse is also true

### Either

# Operations

# Extend the API

You can extend the piping API on the monadic types by implement a functor class and create an instance of the
`operation` class template like so:
```
struct sample_op_fn
{
   template <typename Func>
   constexpr auto operator()(const maybe<int>&, Func some_func);
}

const constexpr operation<sample_op_fn> sample_op = {};
```
