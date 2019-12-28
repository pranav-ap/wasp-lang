#pragma once
#include <optional>
#include <string>
#include <vector>
#include <variant>
#include <memory>

class Scalar;
class Number;
class String;
class Bool;

class Composite;
class Vector;
class Tuple;
class Map;
class Record;

class Optional;
class Variant;

using TypeNode = std::variant<
	std::monostate,
	Scalar, Composite, Optional, Variant,
	Number, String, Bool,
	Vector, Tuple,
	Map, Record
>;

using TypeNode_ptr = std::shared_ptr<TypeNode>;

using KeyTypeNode = std::variant<std::monostate, Number, String>;
using KeyTypeNode_ptr = std::shared_ptr<KeyTypeNode>;

class Type {};

class Scalar : public Type {};

class Composite : public Type {};

class Optional : public Type
{
	std::optional<TypeNode_ptr> optional_type;
};

class Variant : public Type
{
	std::vector<TypeNode_ptr> types;
public:
	Variant(std::vector<TypeNode_ptr> types) : types(types) {};
};

// Scalar Types

class Number : public Scalar {};

class String : public Scalar {};

class Bool : public Scalar {};

// Composite Types

class Vector : public Composite
{
	TypeNode_ptr type;
public:
	Vector(TypeNode_ptr type) : type(type) {};
};

class Tuple : public Composite
{
	std::vector<TypeNode_ptr> types;
public:
	Tuple(std::vector<TypeNode_ptr> types) : types(types) {};
};

class Map : public Composite
{
	KeyTypeNode_ptr key_type;
	TypeNode_ptr value_type;
public:
	Map(KeyTypeNode_ptr key_type, TypeNode_ptr value_type) : key_type(key_type), value_type(value_type) {};
};

class Record : public Composite
{
	std::string type;
public:
	Record(std::string type) : type(type) {};
};
