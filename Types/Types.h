#pragma once

#ifdef TYPES_EXPORTS
#define TYPES_API __declspec(dllexport)
#else
#define TYPES_API __declspec(dllimport)
#endif

#include "pch.h"
#include <string>
#include <memory>

// Type Base

struct TYPES_API Type
{
};

using Type_ptr = std::shared_ptr<Type>;

struct TYPES_API ScalarType : public Type
{
};

struct TYPES_API CompositeType : public Type
{
};

struct TYPES_API OptionalType : public Type
{
	Type_ptr optional_type;
	OptionalType(Type_ptr optional_type) : optional_type(std::move(optional_type)) {};
};

// Scalar Types

struct TYPES_API NumberType : public ScalarType
{
};

struct TYPES_API StringType : public ScalarType
{
};

struct TYPES_API BoolType : public ScalarType
{
};

// Composite Types

struct TYPES_API VectorType : public CompositeType
{
	Type_ptr type;
	VectorType(Type_ptr type) : type(std::move(type)) {};
};

struct TYPES_API UDTType : public CompositeType
{
	std::string name;
	UDTType(std::string name) : name(name) {};
};
