# Its more of a comment really


let x : int = 34
let x : string = "hello"
let x : [int] = [ 1, 2, 3 ]
let x : ⌊ string, string ⌋ = ⌊ "1", "hello" ⌋ # tuple
let x : { string } = { "1", "hello" } # set
let x : ⟨ string => string ⟩ = ⟨ "a" => "b", "c" => "d" ⟩


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
	DOG
	CAT
	enum Bird
		CROW
		PIGEON
	end
end


NAME::LASTNAME::ID3


fn add(a : int, b : int) => int
    if a > b then
        return a
    end

    return b
end


# Planned Features


let x : Person = new Person("john", false)
let [a, ...b, c] = some_list
[a, ...b, c] = some_list


import { print } from 'io'


x.foo.age = 1
x(123).foo(36, gg).age = 1


type Castle
    name: string
end


type length = int
type length = int | string
type WindowStates = "open" | "closed" | "minimized"


if x : int = call() then
    x = 1
else
    3
end
