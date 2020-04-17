# Wasp Language Specification

## Datatypes

- Scalar
    - string
    - bool
    - number
- Composite
    - vector
    - map
    - UDT
    - enum
    - variant
- Generic 


## Declaring variables  
 
let x : str = "hello"
let x : bool = true
let x : num = 5

let x : num | str = 5

let x : [num] = [1, 2, 3]
let x : { str -> str } = { "a" : "b" ,  "c" : "d" }
let x : Person = { a : "b" ,  c : "d" }

## UDT type

```js
type hello {
	foo: bool,
    bar: str
}

let foo : hello = {
  bar : true,
  name : "jim"
}

foo.name
```

## Alias

```js
type length = num
```


# Conditional

```js
if expr {

} else if expr {

} else {

}
```

# Looping

```js
loop { }

for x in vector / map { }

break
continue
```

# Functions

```rust
fn add (a: num, b: num) -> num {
	return a + b
}
```

# Module system

- Everything is private by default.
- use the `pub` keyword

```js
import { foo, bar } from "path/to/filename"
import { foo, bar } from std_lib

pub fn add (a: num, b: num) -> num {
	return a + b
}
```

# Pattern matching

```rust
match expr {
   expr -> {}
}
```

# Enums

```js
enum NAME
{
    ID1,
    ID2
}

NAME::ID1

```

# Builtins 

```js
echo(string)
ask(string, variable)
```
