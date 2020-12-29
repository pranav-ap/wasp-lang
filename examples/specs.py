# Its more of a comment really


let x : int = 34
let x : string = "hello"
let x : [int] = [ 1, 2, 3 ]
let x : ⌊ string ⌋ = ⌊ "1", "hello" ⌋ # tuple
let x : { string } = { "1", "hello" } # set
let x : ⟨ string => string ⟩ = ⟨ "a": "b", "c": "d" ⟩

let [a : int, b : str] = something


a = 1


if a > 3 then
    345
elif a > 3 then
    567
else
    5671
end


if a > 3 then call() else 4


for x : int in [1, 2, 3] do call(x)


for i : int in range(34, 4, 12) do
    print(i)
end


while expr do
    a = a + 4
end


while expr do a = a + 4


enum Animal
	DOG
	CAT
	enum Bird
		CROW
		PIGEON
	end
end


NAME::ID1
NAME::LASTNAME::ID3


type length = int
type length = int | string
type WindowStates = "open" | "closed" | "minimized"


# Planned Features


import { io } from 'io'


fn add(a : int, b : int) => int
    implore a == 4
    swear c > 43

    if a > b then
        return a + b + 4
    end

    return a + b
end


x.foo.age = 1
x(123).foo(36, gg).age = 1


if x : int = call() then
    x = 1
else
    3
end


let x : Person = new Person("john", false)


namepace Hello
end


match { thing, another, third } with
    case { "open", a : int, _ } then
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


let x : (a: int) => int = a + 43
let [a, ...b, c] = some_list


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
    init : ()
    repr : () => string
end


fn Castle::init(name: string)
    self.name = name
end


fn Castle::defend() => any
    x = 43
end

