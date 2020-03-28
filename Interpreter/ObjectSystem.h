#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include <optional>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

struct INTERPRETER_API Object
{
	virtual void print(int level) = 0;
};

using Object_ptr = INTERPRETER_API std::shared_ptr<Object>;

struct INTERPRETER_API ScalarObject : public Object
{
	virtual void print(int level) = 0;
};

struct INTERPRETER_API CompositeObject : public Object
{
	virtual void print(int level) = 0;
};

struct INTERPRETER_API OptionalObject : public Object
{
	std::optional<Object_ptr> value;

	OptionalObject(std::optional<Object_ptr> value) : value(std::move(value)) {};
	void print(int level);
};

using ScalarObject_ptr = INTERPRETER_API std::shared_ptr<ScalarObject>;
using CompositeObject_ptr = INTERPRETER_API std::shared_ptr<CompositeObject>;

// Scalar Objects

struct INTERPRETER_API NumberObject : public ScalarObject
{
	double value;

	NumberObject(double value) : value(value) {};
	void print(int level);
};

struct INTERPRETER_API StringObject : public ScalarObject
{
	std::string value;

	StringObject(std::string value) : value(value) {};
	void print(int level);
};

struct INTERPRETER_API BooleanObject : public ScalarObject
{
	bool value;

	BooleanObject(bool value) : value(value) {};
	void print(int level);
};

using NumberObject_ptr = INTERPRETER_API std::shared_ptr<NumberObject>;
using StringObject_ptr = INTERPRETER_API std::shared_ptr<StringObject>;
using BooleanObject_ptr = INTERPRETER_API std::shared_ptr<BooleanObject>;

// Composite Object

struct INTERPRETER_API VectorObject : public CompositeObject
{
	std::vector<Object_ptr> values;

	VectorObject() {};
	void print(int level);
	void add(Object_ptr value);
};

struct INTERPRETER_API TupleObject : public CompositeObject
{
	std::vector<Object_ptr> values;

	TupleObject() {};
	void print(int level);
	void add(Object_ptr value);
};

struct INTERPRETER_API MapObject : public CompositeObject
{
	std::map<ScalarObject_ptr, Object_ptr> pairs;

	MapObject() {};
	void print(int level);
	void add(ScalarObject_ptr key, Object_ptr value);
};

struct INTERPRETER_API RecordObject : public CompositeObject
{
	std::map<std::string, Object_ptr> pairs;

	RecordObject() {};
	void print(int level);
	void add(std::string key, Object_ptr value);
};

using VectorObject_ptr = INTERPRETER_API std::shared_ptr<VectorObject>;
using TupleObject_ptr = INTERPRETER_API std::shared_ptr<TupleObject>;
using MapObject_ptr = INTERPRETER_API std::shared_ptr<MapObject>;
using RecordObject_ptr = INTERPRETER_API std::shared_ptr<RecordObject>;
