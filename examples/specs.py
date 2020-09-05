let { io } = import('std/io')

// Its more of a comment really

let x : number = 34
let x : string = 'hello'
let x : [number] = [ 1, 2, 3 ]
let x : ⌊ string ⌋ = ⌊ '1', 'hello' ⌋
let x : { string } = { '1', 'hello' }
let x : ⟨ string => string ⟩ = ⟨ 'a': 'b', 'c': 'd' ⟩
let x : Person = new Person('john', false)

a = 1
x.at(32) = 323524

x.foo.age
x(123).foo(36, gg).age


let [a : number, b : string] = c
[a, b] = c

let [a : number, ...b : string, c : string] = some_list
[a, ...b, c] = some_list


if a > 3 and r == 3
    pass
elif a > 3
    pass
else
    pass
end


if let x : number = call()
    pass
else
    pass
end


for x : number in [1, 2, 3]
    pass
end


for id : number in [1, 2, 3]
    pass


for i : number in 1 to 8 step 2 // to, downto
    print(i)
end


fn add(a : number, b : number) => number
    if a > b
        return a + b + 4
    end

    return a + b
end


enum NAME
    ID1
    ID2
    enum LASTNAME
        ID3
        ID4
    enum FIRSTNAME
        ID5
        ID6
end


NAME::ID1
NAME::LASTNAME
NAME::LASTNAME::ID3


type length = number
type length = number | string
type WindowStates = "open" | "closed" | "minimized"


match thing
    case "open"
        echo "open"
    case "closed"
        echo "closed"
end


interface ICastle
    lng: number const
    lat: number const

    tileSize: 8 | 16 | 32

    open_gates: (speed: number) => bool
    close_gates: (speed: number) => bool
end


type Castle ~ ICastle, Building
    name: string

    fn constructor()
        self.name = ''
    end

    fn constructor(name: string)
        self.name = name
    end

    fn repr() => string
        pass
    end
end


operator a : num + b : num => num
    return a + b
end


module Hello

end


A is B



