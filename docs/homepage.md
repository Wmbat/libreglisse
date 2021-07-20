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
with `is_ok()` and `is_err()`.

The monad can be constructed using the `ok` and `err` classes as follows:
```
result<int, std::string> m_ok = ok(1);
result<int, std::string> m_err = err("hello");
```

You can borrow the value stored within the monad when it is an lvalue with the function `borrow()` and the error using
`borrow_err()`
You can take the value stored within the monad when it is an rvalue with the function `take()` and the error using
take_err()`

If you attemp to **borrow** or **take** the value stored when the monad holds an error, an `abort()` will be called. The
inverse is also true

### Either

`either` is a monadic type that, as the name implies, holds one type or another. You can check which side the either
currently hold with `is_left()` and `is_right()`. `either` is essentially a more generic version of `result`.

The monad can be constructed using the `left` and `right` classes as follows:
```
either<int, std::string> m_left = left(1);
either<int, std::string> m_right = right("hello");
```

You can borrow the right value stored within the monad when it is an lvalue with the function `borrow_right()` and the
left value using `borrow_left()`
You can take the right value stored within the monad when it is an rvalue with the function `take_right()` and the left
value using `take_left()`

If you attemp to **borrow** or **take** the value stored on the right when the monad holds a left, an `abort()` will be called. The
inverse is also true

# Operations

Operations is the name given to function that can be aplied on a monadic type to transform, alter or chain sequences of
operations. The `operator|` API is used to resemble unix piping. The API was also built in a way to allow for easy
extensions, as shown in the next [section](#extend-the-api).

The library comes with a set of premade operations  and they are:

* `maybe`:
    * `transform`: Transform the value stored within the monad, if it exists.
    * `and_then`: Chain a function returning a maybe using the value stored, if it exist.
    * `or_else`: Chain a function returning a maybe if no value is stored.
* `result`:
    * `transform`: Transform the value stored within the monad, if it exists.
    * `transform_err`: Transform the error stored within the monad, if it exists.
    * `and_then`: Chain a function returning a result using the value stored, if it exist.
    * `or_else`: Chain a function returning a result using the error stored, if it exist. 
* `either`:
    * `transform_left`: Transform the right value stored within the monad, if it exists.
    * `transform_right`: Transform the left value stored within the monad, if it exists.
    * `flat_transform_left`: Chain a function returning a either using the left value stored, if it exist.
    * `flat_transform_right`: Chain a function returning a either using the right value stored, if it exist. 
    
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
