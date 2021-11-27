# This is a comment


local x : int = 34
local x : string = "hello"
local x : [int] = [ 1, 2, 3 ]
local x : ⌊ string, string ⌋ = ⌊ "1", "hello" ⌋ # tuple
local x : { string } = { "1", "hello" } # set
local x : ⟨ string => string ⟩ = ⟨ "a" => "b", "c" => "d" ⟩


a = 1


if a > 3 then call() else 4


if a > 3 then
    345
elif a > 3 then
    567
else 
    5671
end


while expr do a = a + 4


while expr do
    a = a + 4
end


for x : int in [1, 2, 3] do x


for i : int in [34, 4, 12] do
    i
end


enum Animal
	Dog
	Cat
	enum Bird
		Crow
		Pigeon
	end
end

Animal::Cat


fn add(a : int, b : int) => int
    if a > b then
        return a
    end

    return b
end


if x : int = call() then
    x = 1
else
    3
end


import { echo } from 'io'


native module io
	echo : (text : string) => string
end


native type IntType < AnyType
	to_string : () => string
end


type Castle < Building ~ Account
    name : string
 
    fn new(name: string)
        self.name = name
    end

    fn calc(num: int) => int
        return num + 5 
    end
end


local x = Castle.new("Bastille")


type length = int
type length = int | string
type WindowStates = "open" | "closed" | "minimized"


x.foo.age = 1
x(123).foo(36, gg).age = 1


# Planned Features

interface Account
    name : string
end

delete foo

generate call()

defer call()

defer 
    x = 0
end

let [a : int, ...b : string, c : string] = some_list
[a, ...b, c] = some_list

try 
    throw Error.new("...")

catch e is Animal 
    do_something()

catch e is Any
    do_something_else()
    
finally
    
end

