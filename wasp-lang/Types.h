#pragma once
#include <optional>
#include <string>
#include <vector>

namespace Types {
	class Type {};
	class Scalar : public Type {};
	class Composite : public Type {};

	class Optional : public Type
	{
		std::optional<Type> optional_type;
	};

	// Scalar Types

	class Num : public Scalar {};
	class Str : public Scalar {};
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
}
