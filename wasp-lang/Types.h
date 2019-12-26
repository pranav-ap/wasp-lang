#pragma once
#include <optional>
#include <string>
#include <vector>

class Type {};

class Scalar : public Type {};

class Composite : public Type {};

class Optional : public Type
{
	std::optional<Type> optional_type;
};

class Variant : public Type
{
	std::vector<Type> types;
};

// Scalar Types

class Number : public Scalar {};

class String : public Scalar {};

class Bool : public Scalar {};

// Composite Types

class Vector : public Composite
{
	Type type;
};

class Tuple : public Composite
{
	std::vector<Type> types;
};

class Map : public Composite
{
	Scalar key_type;
	Type value_type;
};

class Record : public Composite
{
	std::string type;
};
