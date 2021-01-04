# Its more of a comment really


let x : int = 34
let x : string = "hello"
let x : [int] = [ 1, 2, 3 ]
let x : [string, string] = ["1", "hello"] # tuple
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


namepace Hello
end


# Planned Features


let x : Person = new Person("john", false)
let [a, ...b, c] = some_list
[a, ...b, c] = some_list


import { io } from 'io'


x.foo.age = 1
x(123).foo(36, gg).age = 1


match { thing, another, third } then
    case { "open", a is int, _ } then
        echo "open"
    case "closed" then
        echo "closed"
    else
        echo "bye"
end


try
catch
else
finally
end


infix + (a : num, b : num) => num
    return a + b
end


interface ICastle
    lng: int
    lat: int

    tileSize: 8 | 16 | 32

    open_gates: (speed: int) => bool
    close_gates: gen (speed: int) => bool
end


type Castle ~ ICastle < Building
    name: string

    private
        constructor : ()
        repr : () => string
end


fn Castle::constructor(name: string)
    self.name = name
end


fn Castle::defend() => any
    x = 43
end


fn add(a : int, b : int) => int
    if a > b then
        return a + b + 4
    end

    return a + b
end


type length = int
type length = int | string
type WindowStates = "open" | "closed" | "minimized"


if x : int = call() then
    x = 1
else
    3
end


scenario "__"
    x = 4

    test "check even"
        assert x == true
    end
end
