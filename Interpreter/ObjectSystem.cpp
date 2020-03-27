#pragma once
#include "pch.h"
#include <iostream>
#include <iomanip>
#include "ObjectSystem.h"

using std::cout;
using std::endl;
using std::setw;
using std::string;

void NumberObject::print(int level)
{
	cout << string(level, ' ') << "Number Object : " << this->value << endl;
}

double NumberObject::get_value()
{
	return value;
}

void StringObject::print(int level)
{
	cout << string(level, ' ') << "String Object : " << this->value << endl;
}

std::string StringObject::get_value()
{
	return value;
}

void BooleanObject::print(int level)
{
	cout << string(level, ' ') << "Boolean Object : " << this->value << endl;
}

bool BooleanObject::get_value()
{
	return value;
}

void OptionalObject::print(int level)
{
	if (this->value)
	{
		cout << string(level + 4, ' ') << "Optional Object : " << this->value.value() << endl;
	}
	else
		cout << string(level + 4, ' ') << "Optional Object : None" << endl;
}

std::optional<Object_ptr> OptionalObject::get_optional_value()
{
	return value;
}

void VectorObject::print(int level)
{
	cout << string(level, ' ') << "Vector Object : " << endl;

	for (auto const& value : this->values)
		value->print(level + 4);
}

std::vector<Object_ptr> VectorObject::get_vector()
{
	return values;
}

void VectorObject::add(Object_ptr value)
{
	values.push_back(value);
}

void TupleObject::print(int level)
{
	cout << string(level, ' ') << "Tuple Object : " << endl;

	for (auto const& value : this->values)
		value->print(level + 4);
}

std::vector<Object_ptr> TupleObject::get_tuple()
{
	return values;
}

void TupleObject::add(Object_ptr value)
{
	values.push_back(value);
}

void MapObject::print(int level)
{
	cout << string(level, ' ') << "Map Object : " << endl;

	for (auto const& [key, value] : this->pairs)
	{
		cout << string(level + 4, ' ') << "Key : " << endl;
		key->print(level + 8);

		cout << string(level + 4, ' ') << "Value : " << endl;
		value->print(level + 8);

		cout << endl;
	}
}

std::map<ScalarObject_ptr, Object_ptr> MapObject::get_map_literal()
{
	return pairs;
}

void MapObject::add(ScalarObject_ptr key, Object_ptr value)
{
	pairs.insert_or_assign(key, value);
}

void RecordObject::print(int level)
{
	cout << string(level, ' ') << "Record Object : " << endl;

	for (auto const& [key, value] : this->pairs)
	{
		cout << string(level + 4, ' ') << "Key : " << key << endl;

		cout << string(level + 4, ' ') << "Value : " << endl;
		value->print(level + 8);

		cout << endl;
	}
}

std::map<std::string, Object_ptr> RecordObject::get_record_literal()
{
	return pairs;
}

void RecordObject::add(std::string key, Object_ptr value)
{
	pairs.insert_or_assign(key, value);
}