# This is a comment


let x : int = 34
let x : string = "hello"
let x : [int] = [ 1, 2, 3 ] # list
let x : ⌊ string, string ⌋ = ⌊ "1", "hello" ⌋ # tuple
let x : { string } = { "1", "hello" } # set
let x : ⟨ string -> string ⟩ = ⟨ "a" -> "b", "c" -> "d" ⟩ # dict

const x : int = 34

x = 1


# Option


let x : opt int = 34


if some x then
    pass
elif no x then
    pass


match x with
    case [x, y] then
        pass    
    case none then
        pass
    case opt y then
        print 'expr is a nested optional variable'
end

let x : bool = match x with [x, y] where x > 43 or with [x, y, z]


# Conditional Flow


if a > 3 then
    345
elif a > 3 then
    567
else 
    5671
end


x = if a > 3 then call() elif a > 6 then 2 else 4


if let x: int = expr then
	5344
else
    3
end


# Matching


match expr with
	case 0 then
		print 'zero'
    case 0 as x then
		print 'zero'

	case [..., _, 0, x] then # can be set, list or tuple
		print 'deconstruct a list'
    case [let ...x, let y, let z: string] then 
        print 'value binding : x, y, z are new variables accessible here'		
    case let [x, y, ...z] then
		print "value binding of all insiders"

	case [x, y] where x <= y then
		print "x, y hold the same value"
    case [x, y] as z where x <= y then
		print "x, y hold the same value"

    case ⟨ let name ⟩ then
        print "$name from a dictionary"
    case ⟨ scores -> [23, 28, let math] ⟩ then
        print "$math from a dictionary"

    case str then
        pass
    case str as x then  # x is the result of expr
        pass

    case Click then  # class
        pass

    case WebEvent::Click then # enum
        pass

	case ["a", "e", "i", "o", "u"] as x then
    	print "It is a vowel"

	case _ then
		print 'default'
end


let x = match expr with
    case 1 then
        return 'a'
	case _ then
        return 'b'
end


# Looping


while expr do a = a + 4
while let x = expr do a = x + 4


while expr do
    a = a + 4
end


until expr do
    a = a + 4
end


until expr do a = a + 4


for x : int in [1, 2, 3] do x


for i : int in [34, 4, 12] do
    i
end


# Function


fn add(a: int, b: int) -> int
    ~ b = 10

    if a > b then
        return a
    end

    return b
end


add(12, 23)
add(a=12, b=23)


# Enum


enum Animal
	Dog
	enum Bird
		Crow
		Pigeon
	end
end


Animal::Cat
Animal::Bird::Crow


# Imports

import io
import io as i
import { print } from io
import { print as pr } from io
import { print, ask } from io
import { send } from 'root/folder/sendmail'
import { send } from './sendmail'


export


# User Defined Data


type length = int
type length = int | string
type WindowStates = "open" | "closed" | "minimized"
type a_tuple = ⌊ string, string ⌋ 


@list ndim
@map string
class Castle extends Building
    name: string

    fn _init_(name: string)
        self.name = name
    end

    fn _getitem_(index: int) -> int
        return self.value.get(index)
    end

    fn _getitem_(index: Index) -> int
        return self.value.get(index)
    end
    
    fn _setitem_(index: int, value: int)
        return self.value.data.store(i, val)
    end

    fn calc(num: int) -> int
        return num + 5 
    end
end


let x = Castle.new("Bastille")


x.foo.age = 1
x(123).foo(36, gg).age = 1
x?.foo.age


# Operator Overloading


operator left: Castle < right: Castle -> bool
    return left.cost < right.cost
end


# Generics


class Box[NameType, AnotherType extends HouseClass]
	name: NameType
 
    fn _init_(name: NameType)
        self.name = name
    end
end


let box = Box[int].new(25)


# Defer


defer call()

defer 
    x = 0
end


# Memory


del foo # delete from memory and remove reference


# Deconstruction


let [a: int, ...b: string, c: string] = some_list
[a, ...b, c] = some_list


# Exception Handling


try
    # do something
    throw Error.new("...")
rescue AnimalException as e
    # handle exception
    do_something(e)
rescue Exception as e
    do_something_else(e)   
else
    # do this if no exception was raised
ensure
    # do this whether or not an exception was raised
 	print "Cleanup..."
end


let x : opt = try someThrowingFunction() 
# x is an optional value if someThrowingFunction raises error
# use if you don't care about error type


# Test Suite


testsuite CastleTestSuite
    description = ''
    tolerance = 0.2
 
    test testname
        assert num > 5
        assert num > 5, 0.2
    end
end


CastleTestSuite.run()
CastleTestSuite.testname.run()


# Native


native module io
	print : (text: string) -> string
end


native type IntType extends AnyType
	to_string : () -> string
end

