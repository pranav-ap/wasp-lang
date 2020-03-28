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

void StringObject::print(int level)
{
	cout << string(level, ' ') << "String Object : " << this->value << endl;
}

void BooleanObject::print(int level)
{
	cout << string(level, ' ') << "Boolean Object : " << this->value << endl;
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

void VectorObject::print(int level)
{
	cout << string(level, ' ') << "Vector Object : " << endl;

	for (auto const& value : this->values)
		value->print(level + 4);
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

void RecordObject::add(std::string key, Object_ptr value)
{
	pairs.insert_or_assign(key, value);
}