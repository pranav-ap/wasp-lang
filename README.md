# Wasp Language Specification

## Datatypes

- Scalar
    - string
    - bool
    - number
- Composite
    - list
    - tensor
    - map
    - UDT
    - enum
    - variant
- Generic 
- any

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
type Person {
    name: str,
	foo: bool
}

let p : Person = {
  name : "jim",
  foo : true
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

for x : num  in [1, 2, 3]
{
    echo(x)
}

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

- Everything is private by default

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
ask()
```
