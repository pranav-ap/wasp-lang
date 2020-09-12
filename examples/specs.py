let { io } = import("std/io") // UNS

// Its more of a comment really

let x : int = 34
let x : string = "hello"
let x : [int] = [ 1, 2, 3 ]
let x : ⌊ string ⌋ = ⌊ "1", "hello" ⌋
let x : { string } = { "1", "hello" }
let x : ⟨ string => string ⟩ = ⟨ "a": "b", "c": "d" ⟩
let x : Person = new Person("john", false)

a = 1
x.at(32) = 323524 // UNS

x.foo.age
x(123).foo(36, gg).age


let [a : int, b : string] = c  // UNS
[a, b] = c  // UNS

let [a : int, ...b : string, c : string] = some_list  // UNS
[a, ...b, c] = some_list  // UNS

if a > 3 and r == 3 then
    345
elif a > 3  then
    567
else
    5671
end


if a > 3 then call() else 4


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


match thing :
    case "open" of string then
        echo "open"
    case "closed" :
        echo "closed"
end


interface ICastle :
    lng: int const
    lat: int const

    tileSize: 8 | 16 | 32

    open_gates: (speed: int) => bool
    close_gates: (speed: int) => bool
end


type Castle ~ ICastle < Building
    name: string

    fn constructor()
        self.name = ""
    end

    fn constructor(name: string)
        self.name = name
    end

    fn repr() => string
        pass
    end
end


operator a : num + b : num => num :
    return a + b
end


module Hello :

end
