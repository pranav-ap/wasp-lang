# This is a comment


let x : int = 34
let x : string = "hello"
let x : [int] = [ 1, 2, 3 ]
let x : ⌊ string, string ⌋ = ⌊ "1", "hello" ⌋ # tuple
let x : { string } = { "1", "hello" } # set
let x : ⟨ string => string ⟩ = ⟨ "a" => "b", "c" => "d" ⟩

const x : int = 34


a = 1


if a > 3 then call() else 4


if a > 3 then
    345
elif a > 3 then
    567
else 
    5671
end


let g = match variable
	case _, a:
		echo a
	case _, a, _:
		echo a
		return a
	case _, a, _: a
end


while expr do a = a + 4


while expr do
    a = a + 4
end


for x : int in [1, 2, 3] do x


for i : int in [34, 4, 12] do
    i
end


fn add(a : int, b : int) => int
    if a > b then
        return a
    end

    return b
end


add(12, 23)
add(a: 12, b: 23)


if x : int = call() then
    x = 1
else
    3
end


# Enum


enum Animal
	Dog
	Cat
	enum Bird
		Crow
		Pigeon
	end
end

Animal::Cat
Animal::Bird::Crow


# Imports


import io
import echo from io
import { echo, ask } from io
import { send } from 'root/folder/sendmail'
import { send } from './sendmail'


# Native


native module io
	echo : (text : string) => string
end


native type IntType < AnyType
	to_string : () => string
end


# User Defined Data


type length = int
type length = int | string
type WindowStates = "open" | "closed" | "minimized"

 
class Castle < Building
    name : string
 
    fn _init_(name: string)
        self.name = name
    end

    fn _copyinit_(name: string)
        self.name = name
    end

    fn _moveinit_(name: string)
        self.name = name
    end

    fn _getitem_(index: int) -> int:
        return self.value.get(index)
    end
    
    fn __setitem__(index: int, value: int):
        return self.value.data.store(i, val)
    end

    fn calc(num: int) => int
        return num + 5 
    end

    operator < (other: Castle) => bool
        return self.name < other.name
    end
end


let x = Castle.new("Bastille")

x.foo.age = 1
x(123).foo(36, gg).age = 1

# Generics

class Box[NameType]
	name : NameType
 
    fn _init_(name: NameType)
        self.name = name
    end
end

let box = Box[int].new(25)


# Planned Features

del foo # delete

gen call() # generate

defer call()

defer 
    x = 0
end

let [a : int, ...b : string, c : string] = some_list
[a, ...b, c] = some_list

# Exception Handling

try 
    throw Error.new("...")
rescue : AnimalException 
    do_something(e)
rescue 
    do_something_else(e)   
ensure    
  echo "Cleanup..."
end


# Test Suite


testsuite CastleTestSuite
    description = ''
    a_tolerance = 0.2
 
    test testname
        assert num > 5
        assert num > 5, 0.2
    end
end


CastleTestSuite.run()
CastleTestSuite.testname.run()

