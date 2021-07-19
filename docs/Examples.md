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

## Flat transformations

### Maybe

#### and_then

#### or_else

### Result

#### and_then

#### or_else

### Either

#### flat_transform_left

#### flat_transform_right
