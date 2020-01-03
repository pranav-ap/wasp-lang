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
	Optional, Variant,
	Number, String, Bool,
	Vector, Tuple,
	Map, Record
>;

using TypeNode_ptr = std::shared_ptr<TypeNode>;

class Type {
public:
	virtual void print(int level) = 0;
};

class Scalar : public Type {
public:
	virtual void print(int level) = 0;
};

class Composite : public Type {
public:
	virtual void print(int level) = 0;
};

class Optional : public Type
{
	TypeNode_ptr optional_type;
public:
	void print(int level);
};

class Variant : public Type
{
	std::vector<TypeNode_ptr> types;
public:
	Variant(std::vector<TypeNode_ptr> types) : types(types) {};
	void print(int level);
};

// Scalar Types

class Number : public Scalar {
public:
	void print(int level);
};

class String : public Scalar {
public:
	void print(int level);
};

class Bool : public Scalar {
public:
	void print(int level);
};

// Composite Types

class Vector : public Composite
{
	TypeNode_ptr type;
public:
	Vector(TypeNode_ptr type) : type(type) {};
	void print(int level);
};

class Tuple : public Composite
{
	std::vector<TypeNode_ptr> types;
public:
	Tuple(std::vector<TypeNode_ptr> types) : types(types) {};
	void print(int level);
};

class Map : public Composite
{
	TypeNode_ptr key_type;
	TypeNode_ptr value_type;
public:
	Map(TypeNode_ptr key_type, TypeNode_ptr value_type) : key_type(key_type), value_type(value_type) {};
	void print(int level);
};

class Record : public Composite
{
	std::string type;
public:
	Record(std::string type) : type(type) {};
	void print(int level);
};

void print_type_node(TypeNode_ptr node, int level);
