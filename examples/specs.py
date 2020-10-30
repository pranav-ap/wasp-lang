let { io } = import("std/io")

// Its more of a comment really

let x : int = 34
let x : string = "hello"
let x : [int] = [ 1, 2, 3 ]
let x : ⌊ string ⌋ = ⌊ "1", "hello" ⌋
let x : { string } = { "1", "hello" }
let x : ⟨ string => string ⟩ = ⟨ "a": "b", "c": "d" ⟩
let x : Person = new Person("john", false)


a = 1
x.foo.age = 1
x(123).foo(36, gg).age = 1



if a > 3 then
    345
elif a > 3 then
    567
else
    5671
end


if a > 3 then call() else 4


if x : int = call() then
    x = 1
else
    3
end


for x : int in [1, 2, 3] do call(x)


for i : int in range(34, 4, 12) do
    print(i)
end


while expr do
    a = a + 4
end


while expr do a = a + 4


fn add(a : int, b : int) => int
    if a > b then
        return a + b + 4
    end

    return a + b
end

enum Animal
	DOG
	CAT
	enum Bird
		CROW
		PIGEON
	end
end


NAME::ID1
NAME::LASTNAME
NAME::LASTNAME::ID3


type length = int
type length = int | string
type WindowStates = "open" | "closed" | "minimized"


interface ICastle
    const foo: int = 43

    lng: int
    lat: int

    tileSize: 8 | 16 | 32

    open_gates: (speed: int) => bool
    close_gates: gen (speed: int) => bool
end


type Castle ~ ICastle < Building
    const foo: int = 43

    name: string

    constructor : ()
    constructor : (name: string)
    repr : () => string
end


fn Castle::constructor(name: string)
    self.name = name
end


fn Castle::defend() => any
    x = 43
end


infix + (a : num, b : num) => num
    return a + b
end


namepace Hello
end


// Planned Features

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


let [a : int, ...b : string, c : string] = some_list
[a, ...b, c] = some_list