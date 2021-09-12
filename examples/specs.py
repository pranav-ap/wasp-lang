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


while expr do
    a = a + 4
end


while expr do a = a + 4


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


native io
	echo : (text : string) => string
end


# Planned Features

type length = int
type length = int | string
type WindowStates = "open" | "closed" | "minimized"


interface Account
    name : string
end


type Castle < Building ~ Account
    name : string

    new : (string)
    calc : (int) => int
end


fn Castle::new(name: string)
    self.name = name
end


fn Castle::calc(num: int) => int
    return num + 5
end


let x = Castle.new("Bastille")


x.foo.age = 1
x(123).foo(36, gg).age = 1


infix + (a : num, b : num) => num
    return a + b
end


let [a : int, ...b : string, c : string] = some_list
[a, ...b, c] = some_list

