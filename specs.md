# Wasp Language Specification

## Fundamental Datatypes

- Scalar - str, bool, num
- Composite - vector, map, record, enum
- Optional - opt

## Variables

- all variables need to be initialized on declaration

let a : num = 5;
a;

let a : [num] = [5];
a[3];

let a : opt [opt num] = [5];
a[3];

let a : (num, str) = (5, "john");
a[2];

let a : { str -> str } = { "name": "john" };
a["name"];

- key cannot be optional

## Record type

type hello {
	foo: bool,
  bar: str
}

let foo : hello = {
  bar : true,
  name : "jim"
};

foo.name;

## Alias

type length = int;

# Comments

Comments begin with `//`.

# Conditional

if expr {

} else if expr {

} else {

}

# Looping

loop { }

for x in vector { }

break;
continue;
s
# Functions

fn add (a: num, b: num) -> num {
	return a + b;
}

# Module system

- Everything is private by default.
- use the `pub` keyword

import { foo, bar } from "path/to/filename";

import { foo, bar } from "path/to/folder_name";
- folder must have a file named mod.wasp

pub fn add (a: num, b: num) -> num {
	return a + b;
}
 