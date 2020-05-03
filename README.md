# Wasp Language Specification

## Datatypes

- Any
    - Scalar
        - string
        - bool
        - number
    - Composite
        - list
        - tuple
        - map
        - UDT
        - enum
        - variant
    - None
    - Function


## Declaring variables  

let x : any = "hello"

let x : str = "hello"
let x : bool = true
let x : num = 5

let x : [num] = [1, 2, 3]
let x : (num, str) = (@ 1, 'hi')
let x : (num) = (@ 1)
let x : { str -> str } = { "a" : "b" ,  "c" : "d" }
let x : Person = { a : "b" ,  c : "d" }
let x : num | str = 5

syntactic sugar - opt 

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
if expr:
    pass
elif expr:
    pass
else:
    pass
```

# Looping

```js
while condition:
    let x = 4

for x : num in [1, 2, 3]:
    echo(x)

break
continue
```

# Functions

```rust
fn add (a: num, b: num) : num {
	return a + b
}
```

# Module system

```js
import { foo, bar } from "path/to/filename"
import { foo, bar } from std_lib
```

# Enums

```js
enum NAME
{
    ID1,
    ID2,
    LASTNAME
    {
        ID3,
        ID4
	}
}

NAME::ID1
NAME::LASTNAME::ID3
```

# Builtins 

```js
echo(string)
ask()
```

