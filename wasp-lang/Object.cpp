#pragma once
#include <iostream>
#include <iomanip>
#include "Object.h"

using std::cout;
using std::endl;
using std::setw;
using std::visit;
using std::string;

void print_object(ObjectVariant_ptr object, int level)
{
	visit([level](auto obj) {
		using T = std::decay_t<decltype(obj)>;

		if constexpr (std::is_same_v<T, NumberObject>)
			obj.print(level);
		else if constexpr (std::is_same_v<T, StringObject>)
			obj.print(level);
		else if constexpr (std::is_same_v<T, BooleanObject>)
			obj.print(level);
		else if constexpr (std::is_same_v<T, VectorObject>)
			obj.print(level);
		else if constexpr (std::is_same_v<T, TupleObject>)
			obj.print(level);
		else if constexpr (std::is_same_v<T, MapObject>)
			obj.print(level);
		else if constexpr (std::is_same_v<T, RecordObject>)
			obj.print(level);
		else if constexpr (std::is_same_v<T, OptionalObject>)
			obj.print(level);
		else if constexpr (std::is_same_v<T, VariantObject>)
			obj.print(level);
		}, *object.get());
}

void OptionalObject::print(int level)
{
	if (this->value)
	{
		cout << string(level + 4, ' ') << "Optional Object : " << endl;
		print_object(this->value.value(), level + 4);
	}
	else
		cout << string(level + 4, ' ') << "Optional Object : None" << endl;
}

void VariantObject::print(int level)
{
	cout << string(level + 4, ' ') << "Variant Object : " << endl;
	print_object(this->value, level + 4);
}

void NumberObject::print(int level)
{
	cout << string(level, ' ') << "Number Object : " << this->value << endl;
}

void StringObject::print(int level)
{
	cout << string(level, ' ') << "String Object : " << this->value << endl;
}

void BooleanObject::print(int level)
{
	cout << string(level, ' ') << "Boolean Object : " << this->value << endl;
}

void VectorObject::print(int level)
{
	cout << string(level, ' ') << "Vector Object : " << endl;

	for (auto const& value : this->values)
		print_object(value, level + 4);
}

void TupleObject::print(int level)
{
	cout << string(level, ' ') << "Tuple Object : " << endl;

	for (auto const& value : this->values)
		print_object(value, level + 4);
}

void MapObject::print(int level)
{
	cout << string(level, ' ') << "Map Object : " << endl;

	for (auto const& value : this->values)
	{
		cout << string(level + 4, ' ') << "Key : " << endl;
		print_object(value.first, level + 8);

		cout << string(level + 4, ' ') << "Value : " << endl;
		print_object(value.second, level + 8);

		cout << endl;
	}
}

void RecordObject::print(int level)
{
	cout << string(level, ' ') << "Record Object : " << endl;

	for (auto const& value : this->values)
	{
		cout << string(level + 4, ' ') << "Key : " << value.first << endl;

		cout << string(level + 4, ' ') << "Value : " << endl;
		print_object(value.second, level + 8);

		cout << endl;
	}
}