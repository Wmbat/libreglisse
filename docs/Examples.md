# Examples

## transformations

### Maybe

#### transform

We can transform the value stored in a maybe by piping the `transform` operation as follows. It transforms the value
using a function or lambda to modify, alter or change the value & type store in the monad.

```cpp
using namespace reglisse;

maybe<int> m_int = some(1);
maybe<std::string> m_str = m_int | transform([](int i){ return std::to_string(i); });
```

You can see that the monad **m_str** hold a value of type string, which is the return type of the lambda passed in
`transform`

Do note that if the `maybe` is empty, the `transform` will not do anything to the value.

### Result

#### transform

`transform` also work on `result` monads in the same way.

```cpp
using namespace reglisse;

result<int, float> r_int = ok(1);
result<std::string, float> = r_str = r_int | transform([](int i){ return std::to_string(i); });
```

You can see that the syntax & behaviour is the same as shown above for the maybe monad.    

Do note that if the `result` contains an error, the `transform` will not do anything to the value.

#### transform_err

`transform_err` does the same thing as `transform` but on the error side of the result.

```cpp
using namespace reglisse;

result<int, float> r_int = ok(1);
result<int, std::string> = r_str = r_int 
    | transform_err([](int i) -> std::string { return i == 0 ? "hello" : "world"; });
```

Do note that if the `result` contains a value, the `transform_err` will not do anything to the error.

### Either

#### transform_left

#### transform_right

## Join transformations

### Maybe

#### and_then

#### or_else

### Result

#### and_then

#### or_else

### Either

#### transform_join_left

`transform_join_left` takes for input a function who's input is the left hand type of the `either` monad. That function
must also return an `either` which will be joined with the current monad with left hand type being determined by the
transformation and the right hand type being the same as the original monad.

```cpp
using namespace reglisse;

either<int, std::string> e_int = left(1);
either<float, std::string> e_res = e_int 
   | transform_join_left([](int i) -> either<float, std::string> {
         return left(static_cast<float>(i));
      });
```

Do note that if the original monad (e_int) in the example contained a value in it's right, the `transform_join_left` operation
actually call the input function, but it will modify the `either`'s left type.

#### transform_join_right

`transform_join_right` takes for input a function who's input is the right hand type of the `either` monad. That
function must also return an `either` which will be joined with the current monad with the right hand type determined by
the transformation and the left hand type being the same as the original monad.

```cpp
using namespace reglisse;

either<int, std::string> e_str = right("hello");
either<float, std::string> e_res = e_str
   | transform_join_left([](std::string& i) -> either<int, std::string_view> {
         return right(std::string_view(i));
      });
```

Do note that if the original monad (e_str) in the example contained a value in it's lfet, the `transform_join_right` operation
actually call the input function, but it will modify the `either`'s right type.
