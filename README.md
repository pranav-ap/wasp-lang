# Wasp Language Specification

## Fundamental Datatypes

- Scalar - str, bool, num
- Composite - vector, map, UDT, enum
- Optional - opt

## Variables

- all variables need to be initialized on declaration

```js
let a : num = 5
a

let a : [num] = [5]
a[3]

let a : opt [opt num] = [5]
a[3]

let a : { str -> str } = { "name": "john" }
a["name"]
```

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

## Variant

```js
let a : <str, int, bool> = 0

let a : [str, <int, bool>] = ["hello", 0]
```

# Comments

Comments begin with `//`.

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

# Built in core library

```js
echo(string)
ask(string, variable)
```
