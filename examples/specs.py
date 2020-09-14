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


let [a : int, ...b : string, c : string] = some_list
[a, ...b, c] = some_list


if a > 3 and r == 3 then
    345
elif a > 3  then
    567
else
    5671
end


if a > 3 then call() else 4


// UC
if let x : int = call() :
    pass
else :
    pass
end


for x : int in [1, 2, 3] do call(x)


for i : int in 1 to 8 step 2 do // to, downto
    print(i)
end

while expr do
    a = a + 4
end


while expr do a = a + 4


fn add(a : mut int, b : mut int) => int
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


match thing then
    case "open" : string then
        echo "open"
    case "closed" then
        echo "closed"
    else
        echo "bye"
end


interface ICastle :
    lng: int const
    lat: int const

    tileSize: 8 | 16 | 32

    open_gates: (speed: int) => bool
    close_gates: gen (speed: int) => bool
end

// UC

type Castle ~ ICastle < Building
    name: string

    constructor : fn ()
    constructor : (name: string)
    repr : () => string
end


fn constructor(name: string)
    self.name = name
end


fn Castle::defend() => any
    x = 43
end


operator a : num + b : num => num :
    return a + b
end


module Hello
end


try
catch
else
finally
end
