#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include <optional>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

struct OBJECTSYSTEM_API Object
{
	virtual void accept() = 0;
};

using Object_ptr = OBJECTSYSTEM_API std::shared_ptr<Object>;

struct OBJECTSYSTEM_API ScalarObject : public Object
{
	virtual void accept() = 0;
};

struct OBJECTSYSTEM_API CompositeObject : public Object
{
	virtual void accept() = 0;
};

struct OBJECTSYSTEM_API OptionalObject : public Object
{
	std::optional<Object_ptr> value;
	OptionalObject(std::optional<Object_ptr> value) : value(std::move(value)) {};
	void accept();
};

using ScalarObject_ptr = OBJECTSYSTEM_API std::shared_ptr<ScalarObject>;
using CompositeObject_ptr = OBJECTSYSTEM_API std::shared_ptr<CompositeObject>;
using OptionalObject_ptr = OBJECTSYSTEM_API std::shared_ptr<OptionalObject>;

// Scalar Objects

struct OBJECTSYSTEM_API NumberObject : public ScalarObject
{
	double value;
	NumberObject(double value) : value(value) {};
	void accept();
};

struct OBJECTSYSTEM_API StringObject : public ScalarObject
{
	std::string value;
	StringObject(std::string value) : value(value) {};
	void accept();
};

struct OBJECTSYSTEM_API BooleanObject : public ScalarObject
{
	bool value;
	BooleanObject(bool value) : value(value) {};
	void accept();
};

// Composite Object

struct OBJECTSYSTEM_API VectorObject : public CompositeObject
{
	std::vector<Object_ptr> values;

	VectorObject() {};
	void add(Object_ptr value);
	void accept();
};

struct OBJECTSYSTEM_API MapObject : public CompositeObject
{
	std::map<Object_ptr, Object_ptr> pairs; //??? change key type

	MapObject() {};
	void add(Object_ptr key, Object_ptr value);
	void accept();
};

struct OBJECTSYSTEM_API UDTObject : public CompositeObject
{
	std::map<std::string, Object_ptr> pairs;

	UDTObject() {};
	void add(std::string key, Object_ptr value);
	void accept();
};

using NumberObject_ptr = OBJECTSYSTEM_API std::shared_ptr<NumberObject>;
using StringObject_ptr = OBJECTSYSTEM_API std::shared_ptr<StringObject>;
using BooleanObject_ptr = OBJECTSYSTEM_API std::shared_ptr<BooleanObject>;

using VectorObject_ptr = OBJECTSYSTEM_API std::shared_ptr<VectorObject>;
using MapObject_ptr = OBJECTSYSTEM_API std::shared_ptr<MapObject>;
using UDTObject_ptr = OBJECTSYSTEM_API std::shared_ptr<UDTObject>;

OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const NumberObject_ptr obj);
OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const StringObject_ptr obj);
OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const BooleanObject_ptr obj);
OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const VectorObject_ptr obj);
OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const MapObject_ptr obj);
OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const UDTObject_ptr obj);
