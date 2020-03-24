#pragma once

#ifdef INTERPRETER_EXPORTS
#define INTERPRETER_API __declspec(dllexport)
#else
#define INTERPRETER_API __declspec(dllimport)
#endif

#include <optional>
#include <string>
#include <vector>
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

class INTERPRETER_API CompositeObject : public Object
{
public:
	virtual void print(int level) = 0;
};

class INTERPRETER_API OptionalObject : public Object
{
	std::optional<Object_ptr> value;
public:
	OptionalObject(std::optional<Object_ptr> value) : value(std::move(value)) {};
	void print(int level);
};

// Scalar Object

class INTERPRETER_API NumberObject : public ScalarObject
{
	double value;
public:
	NumberObject(double value) : value(value) {};
	void print(int level);
};

class INTERPRETER_API StringObject : public ScalarObject
{
	std::string value;
public:
	StringObject(std::string value) : value(value) {};
	void print(int level);
};

class INTERPRETER_API BooleanObject : public ScalarObject
{
	bool value;
public:
	BooleanObject(bool value) : value(value) {};
	void print(int level);
};

// Composite Object

class INTERPRETER_API VectorObject : public CompositeObject
{
	std::vector<Object_ptr> values;
public:
	VectorObject(std::vector<Object_ptr> values) : values(values) {};
	void print(int level);
};

class INTERPRETER_API TupleObject : public CompositeObject
{
	std::vector<Object_ptr> values;
public:
	TupleObject(std::vector<Object_ptr> values) : values(values) {};
	void print(int level);
};

class INTERPRETER_API MapObject : public CompositeObject
{
	std::vector<std::pair<Object_ptr, Object_ptr>> values;
public:
	MapObject(std::vector<std::pair<Object_ptr, Object_ptr>> values) : values(values) {};
	void print(int level);
};

class INTERPRETER_API RecordObject : public CompositeObject
{
	std::vector<std::pair<std::string, Object_ptr>> values;
public:
	RecordObject(std::vector<std::pair<std::string, Object_ptr>> values) : values(values) {};
	void print(int level);
};
