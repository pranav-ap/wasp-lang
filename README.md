# Wasp Language Specification

## Fundamental Datatypes

- Scalar - str, bool, num
- Composite - vector, map, record, enum
- Optional - opt

## Variables

- all variables need to be initialized on declaration

let a : num = 5
a

let a : [num] = [5]
a[3]

let a : opt [opt num] = [5]
a[3]

let a : (num, str) = (5, "john")
a[2]

let a : { str -> str } = { "name": "john" }
a["name"]

- key cannot be optional

## Record type

type hello {
	foo: bool,
    bar: str
}

let foo : hello = {
  bar : true,
  name : "jim"
}

foo.name

## Alias

type length = num

## Variant

let a : <str, int, bool> = 0

let a : [str, <int, bool>] = ["hello", 0]

# Comments

Comments begin with `//`.

# Conditional

if expr {

} else if expr {

} else {

}

# Looping

loop { }

for x in vector / map { }

break;
continue;

# Functions

fn add (a: num, b: num) -> num {
	return a + b
}

# Module system

- Everything is private by default.
- use the `pub` keyword

import { foo, bar } from "path/to/filename"

pub fn add (a: num, b: num) -> num {
	return a + b
}
 
 # Pattern matching

 match expr {
    expr -> {}
 }



# Code Guidelines

- Every function is responsible for pointing to the next unexplored 
character or token before transfering control