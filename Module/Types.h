#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include <string>
#include <vector>
#include <memory>

// Type Base

struct MODULE_API Type
{
};

using Type_ptr = std::shared_ptr<Type>;

struct MODULE_API Scalar : public Type
{
};

struct MODULE_API Composite : public Type
{
};

struct MODULE_API Optional : public Type
{
	Type_ptr optional_type;
	Optional(Type_ptr optional_type) : optional_type(std::move(optional_type)) {};
};

// Scalar Types

struct MODULE_API Number : public Scalar
{
};

struct MODULE_API String : public Scalar
{
};

struct MODULE_API Bool : public Scalar
{
};

// Composite Types

struct MODULE_API Vector : public Composite
{
	Type_ptr type;
	Vector(Type_ptr type) : type(std::move(type)) {};
};

struct MODULE_API Map : public Composite
{
	Type_ptr key_type;
	Type_ptr value_type;
	Map(Type_ptr key_type, Type_ptr value_type) : key_type(std::move(key_type)), value_type(std::move(value_type)) {};
};

struct MODULE_API UDT : public Composite
{
	std::string type_name;
	UDT(std::string type_name) : type_name(type_name) {};
};
