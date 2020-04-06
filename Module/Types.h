#pragma once

#ifdef MODULE_EXPORTS
#define MODULE_API __declspec(dllexport)
#else
#define MODULE_API __declspec(dllimport)
#endif

#include "pch.h"
#include <string>
#include <memory>

// Type Base

struct MODULE_API Type
{
};

using Type_ptr = std::shared_ptr<Type>;

struct MODULE_API ScalarType : public Type
{
};

struct MODULE_API CompositeType : public Type
{
};

struct MODULE_API OptionalType : public Type
{
	Type_ptr optional_type;
	OptionalType(Type_ptr optional_type) : optional_type(std::move(optional_type)) {};
};

// Scalar Types

struct MODULE_API NumberType : public ScalarType
{
};

struct MODULE_API StringType : public ScalarType
{
};

struct MODULE_API BooleanType : public ScalarType
{
};

// Composite Types

struct MODULE_API VectorType : public CompositeType
{
	Type_ptr type;
	VectorType(Type_ptr type) : type(std::move(type)) {};
};

struct MODULE_API UDTType : public CompositeType
{
	std::string name;
	UDTType(std::string name) : name(name) {};
};
