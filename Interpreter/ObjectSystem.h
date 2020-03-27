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

class INTERPRETER_API Object
{
public:
	virtual void print(int level) = 0;
};

using Object_ptr = std::shared_ptr<Object>;

class INTERPRETER_API ScalarObject : public Object
{
public:
	virtual void print(int level) = 0;
};

using ScalarObject_ptr = std::shared_ptr<ScalarObject>;

class INTERPRETER_API CompositeObject : public Object
{
public:
	virtual void print(int level) = 0;
};

using CompositeObject_ptr = std::shared_ptr<CompositeObject>;

class INTERPRETER_API OptionalObject : public Object
{
	std::optional<Object_ptr> value;
public:
	OptionalObject() {};
	OptionalObject(std::optional<Object_ptr> value) : value(std::move(value)) {};
	void print(int level);
	std::optional<Object_ptr> get_optional_value();
};

// Scalar Objects

class INTERPRETER_API NumberObject : public ScalarObject
{
	double value;
public:
	NumberObject() : value(0) {};
	NumberObject(double value) : value(value) {};
	void print(int level);
	double get_value();
};

class INTERPRETER_API StringObject : public ScalarObject
{
	std::string value;
public:
	StringObject() {};
	StringObject(std::string value) : value(value) {};
	void print(int level);
	std::string get_value();
};

class INTERPRETER_API BooleanObject : public ScalarObject
{
	bool value;
public:
	BooleanObject() : value(false) {};
	BooleanObject(bool value) : value(value) {};
	void print(int level);
	bool get_value();
};

// Composite Object

class INTERPRETER_API VectorObject : public CompositeObject
{
	std::vector<Object_ptr> values;
public:
	VectorObject() {};
	VectorObject(std::vector<Object_ptr> values) : values(values) {};
	void print(int level);

	std::vector<Object_ptr> get_vector();
	void add(Object_ptr value);
};

class INTERPRETER_API TupleObject : public CompositeObject
{
	std::vector<Object_ptr> values;
public:
	TupleObject() {};
	TupleObject(std::vector<Object_ptr> values) : values(values) {};
	void print(int level);

	std::vector<Object_ptr> get_tuple();
	void add(Object_ptr value);
};

class INTERPRETER_API MapObject : public CompositeObject
{
	std::map<ScalarObject_ptr, Object_ptr> pairs;
public:
	MapObject() {};
	MapObject(std::map<ScalarObject_ptr, Object_ptr> pairs) : pairs(pairs) {};
	void print(int level);

	std::map<ScalarObject_ptr, Object_ptr> get_map_literal();
	void add(ScalarObject_ptr key, Object_ptr value);
};

class INTERPRETER_API RecordObject : public CompositeObject
{
	std::map<std::string, Object_ptr> pairs;
public:
	RecordObject() {};
	RecordObject(std::map<std::string, Object_ptr> pairs) : pairs(pairs) {};
	void print(int level);

	std::map<std::string, Object_ptr> get_record_literal();
	void add(std::string key, Object_ptr value);
};
