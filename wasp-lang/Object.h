#pragma once
#include <optional>
#include <string>
#include <vector>
#include <utility>
#include <variant>
#include <memory>

class ScalarObject;

class NumberObject;
class StringObject;
class BooleanObject;

class CompositeObject;

class VectorObject;
class TupleObject;

class MapObject;
class RecordObject;

class OptionalObject;
class VariantObject;

using ObjectVariant = std::variant<
	std::monostate,
	NumberObject, StringObject, BooleanObject,
	VectorObject, TupleObject,
	MapObject, RecordObject,
	OptionalObject, VariantObject
>;

using ObjectVariant_ptr = std::shared_ptr<ObjectVariant>;

class Object
{
public:
	virtual void print(int level) = 0;
};

class ScalarObject : public Object
{
public:
	virtual void print(int level) = 0;
};

class CompositeObject : public Object
{
public:
	virtual void print(int level) = 0;
};

class OptionalObject : public Object
{
	std::optional<ObjectVariant_ptr> value;
public:
	OptionalObject(std::optional<ObjectVariant_ptr> value) : value(value) {};
	void print(int level);
};

class VariantObject : public Object
{
	ObjectVariant_ptr value;
public:
	VariantObject(ObjectVariant_ptr value) : value(value) {};
	void print(int level);
};

// Scalar Object

class NumberObject : public ScalarObject
{
	double value;
public:
	NumberObject(double value) : value(value) {};
	void print(int level);
};

class StringObject : public ScalarObject
{
	std::string value;
public:
	StringObject(std::string value) : value(value) {};
	void print(int level);
};

class BooleanObject : public ScalarObject
{
	bool value;
public:
	BooleanObject(bool value) : value(value) {};
	void print(int level);
};

// Composite Object

class VectorObject : public CompositeObject
{
	std::vector<ObjectVariant_ptr> values;
public:
	VectorObject(std::vector<ObjectVariant_ptr> values) : values(values) {};
	void print(int level);
};

class TupleObject : public CompositeObject
{
	std::vector<ObjectVariant_ptr> values;
public:
	TupleObject(std::vector<ObjectVariant_ptr> values) : values(values) {};
	void print(int level);
};

class MapObject : public CompositeObject
{
	std::vector<std::pair<ObjectVariant_ptr, ObjectVariant_ptr>> values;
public:
	MapObject(std::vector<std::pair<ObjectVariant_ptr, ObjectVariant_ptr>> values) : values(values) {};
	void print(int level);
};

class RecordObject : public CompositeObject
{
	std::vector<std::pair<std::string, ObjectVariant_ptr>> values;
public:
	RecordObject(std::vector<std::pair<std::string, ObjectVariant_ptr>> values) : values(values) {};
	void print(int level);
};

void print_object(ObjectVariant_ptr object, int level);
