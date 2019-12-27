#pragma once
#include <optional>
#include <string>
#include <vector>
#include <variant>
#include <memory>

class Scalar;
class Composite;
class Optional;
class Variant;
class Number;
class String;
class Bool;
class Vector;
class Tuple;
class Map;
class Record;

using TypeNode = std::variant<
	Scalar, Composite, Optional, Variant,
	Number, String, Bool,
	Vector, Tuple,
	Map, Record
>;

using Type_ptr = std::shared_ptr<TypeNode>;

class Type {};

class Scalar : public Type {};

class Composite : public Type {};

class Optional : public Type
{
	std::optional<Type_ptr> optional_type;
};

class Variant : public Type
{
	std::vector<Type_ptr> types;
};

// Scalar Types

class Number : public Scalar {};

class String : public Scalar {};

class Bool : public Scalar {};

// Composite Types

class Vector : public Composite
{
	Type_ptr type;
};

class Tuple : public Composite
{
	std::vector<Type_ptr> types;
};

class Map : public Composite
{
	Scalar key_type;
	Type_ptr value_type;
};

class Record : public Composite
{
	std::string type;
public:
	Record(std::string type) : type(type) {};
};
