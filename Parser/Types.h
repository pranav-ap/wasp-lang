#pragma once

#ifdef PARSER_EXPORTS
#define PARSER_API __declspec(dllexport)
#else
#define PARSER_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <memory>

// Type Base Classes

class PARSER_API Type
{
public:
	virtual void print(int level) = 0;
};

using Type_ptr = std::shared_ptr<Type>;

class PARSER_API Scalar : public Type
{
public:
	virtual void print(int level) = 0;
};

class PARSER_API Composite : public Type
{
public:
	virtual void print(int level) = 0;
};

class PARSER_API Optional : public Type
{
	Type_ptr optional_type;
public:
	Optional(Type_ptr optional_type) : optional_type(std::move(optional_type)) {};
	void print(int level);
};

// Scalar Types

class PARSER_API Number : public Scalar
{
public:
	void print(int level);
};

class PARSER_API String : public Scalar
{
public:
	void print(int level);
};

class PARSER_API Bool : public Scalar
{
public:
	void print(int level);
};

// Composite Types

class PARSER_API Vector : public Composite
{
	Type_ptr type;
public:
	Vector(Type_ptr type) : type(std::move(type)) {};
	void print(int level);
};

class PARSER_API Tuple : public Composite
{
	std::vector<Type_ptr> types;
public:
	Tuple(std::vector<Type_ptr> types) : types(types) {};
	void print(int level);
};

class PARSER_API Map : public Composite
{
	Type_ptr key_type;
	Type_ptr value_type;
public:
	Map(Type_ptr key_type, Type_ptr value_type) : key_type(std::move(key_type)), value_type(std::move(value_type)) {};
	void print(int level);
};

class PARSER_API Record : public Composite
{
	std::string type_name;
public:
	Record(std::string type_name) : type_name(type_name) {};
	void print(int level);
};
