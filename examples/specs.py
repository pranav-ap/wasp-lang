# This is a comment


let x : int = 34
let x : string = "hello"
let x : [int] = [ 1, 2, 3 ] # list
let x : ⌊ roll: string, name: string ⌋ = ⌊ "1", "hello" ⌋ # tuple
let x : { string } = { "1", "hello" } # set
let x : ⟨ string => string ⟩ = ⟨ "a" => "b", "c" => "d" ⟩ # dict


const x : int = 34
let x : opt int = 34


x = 1


# Conditional Flow


if a > 3 then
    345
elif a > 3 then
    567
else 
    5671
end


x = if a > 3 then call() else 4


if let x = expr then
	5344

    
if x: int = call() then
    x = 1
else
    3
end



# Matching


match expr
	case 0:
		print 'zero'

	case [..., _, 0, x]: # can be set, list or tuple
		print 'deconstruct a list'
    case [let ...x, let y, let z: string]: 
        print 'value binding : x, y, z are new variables accessible here'		
    case let [x, y, ...z]:
		print "value binding of all insiders"
	case [x, y] where x <= y:
		print "x, y hold the same value"

    case ⟨ let name ⟩:
        print "$name from a dictionary"
    case ⟨ scores => [23, 28, let math] ⟩:
        print "$math from a dictionary"

    case Click e:  # class
        pass
    case WebEvent::Click e: # class within enum
        pass

	case "a", "e", "i", "o", "u":
    	print "It is a vowel"

	default:
		return a
end


let x = match expr
    case 1:
        return 'a'
    default:
        return 'b'
end


# Looping


loop
    something()
end


loop something()


while expr do a = a + 4


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


fn add(a: int, b: int = 10): int
    if a > b then
        return a
    end

    return b
end


add(12, 23)
add(a:12, b:23)


# Enum


enum Animal
	Dog
	class Cat
    type a_tuple
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
import print from io
import print as pr from io
import { print, ask } from io
import { send } from 'root/folder/sendmail'
import { send } from './sendmail'


export


# User Defined Data


type length = int
type length = int | string
type WindowStates = "open" | "closed" | "minimized"
type a_tuple = ⌊ roll: string, name: string ⌋ 


class Castle extends Building impl Iterable
    name: string
    name: static string
 
    fn _staticinit_()
        Castle.name = 'jon'
    end

    fn _init_(name: string)
        self.name = name
    end

    fn _getitem_(index: int) = int
        return self.value.get(index)
    end
    
    fn _setitem_(index: int, value :int)
        return self.value.data.store(i, val)
    end

    fn calc(num :int) :int
        return num + 5 
    end
	
    static fn another_calc(num: int): int
        return num + 5 
    end
end


let x = Castle.new("Bastille")


x.foo.age = 1
x(123).foo(36, gg).age = 1
x?foo.age


# Operator Overloading


operator left: Castle < right: Castle => bool
    return left.cost < right.cost
end


# Generics


class Box[NameType]
	name: NameType
 
    fn _init_(name: NameType)
        self.name = name
    end
end


let box = Box[int].new(25)


# Interface


interface Iterator
    nextIndex: int
    
    fn _init_(start: int = 0, end: int = Infinity, step: int = 1)
        self.nextIndex = start
    end

    fn next()
        pass
        # nextIndex += step
        # iterationCount++
        # return { value: iterationCount, done: true }
    end

end


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
rescue e : AnimalException 
    # handle exception
    do_something(e)
rescue e : Exception
    do_something_else(e)   
else
    # do this if no exception was raised
ensure
    # do this whether or not an exception was raised
 	print "Cleanup..."
end


let x = try someThrowingFunction() 
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
	print: (text : string): string
end


native type IntType < AnyType
	to_string : (): string
end

