#pragma once
#include <string>
#include <vector>
#include <memory>

// Type Base Classes

class Type
{
public:
	virtual void print(int level) = 0;
};

using Type_ptr = std::shared_ptr<Type>;

class Scalar : public Type
{
public:
	virtual void print(int level) = 0;
};

class Composite : public Type
{
public:
	virtual void print(int level) = 0;
};

class Optional : public Type
{
	Type_ptr optional_type;
public:
	Optional(Type_ptr optional_type) : optional_type(std::move(optional_type)) {};
	void print(int level);
};

// Scalar Types

class Number : public Scalar
{
public:
	void print(int level);
};

class String : public Scalar
{
public:
	void print(int level);
};

class Bool : public Scalar
{
public:
	void print(int level);
};

// Composite Types

class Vector : public Composite
{
	Type_ptr type;
public:
	Vector(Type_ptr type) : type(std::move(type)) {};
	void print(int level);
};

class Tuple : public Composite
{
	std::vector<Type_ptr> types;
public:
	Tuple(std::vector<Type_ptr> types) : types(types) {};
	void print(int level);
};

class Map : public Composite
{
	Type_ptr key_type;
	Type_ptr value_type;
public:
	Map(Type_ptr key_type, Type_ptr value_type) : key_type(std::move(key_type)), value_type(std::move(value_type)) {};
	void print(int level);
};

class Record : public Composite
{
	std::string type;
public:
	Record(std::string type) : type(type) {};
	void print(int level);
};
