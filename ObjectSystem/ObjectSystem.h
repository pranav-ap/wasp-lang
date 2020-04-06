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

class ObjectVisitor;

struct OBJECTSYSTEM_API Object
{
	virtual void accept(ObjectVisitor& visitor) = 0;
};

using Object_ptr = OBJECTSYSTEM_API std::shared_ptr<Object>;

struct OBJECTSYSTEM_API ScalarObject : public Object
{
	virtual void accept(ObjectVisitor& visitor) = 0;
};

struct OBJECTSYSTEM_API CompositeObject : public Object
{
	virtual void accept(ObjectVisitor& visitor) = 0;
};

struct OBJECTSYSTEM_API OptionalObject : public Object, public std::enable_shared_from_this<OptionalObject>
{
	std::optional<Object_ptr> value;
	OptionalObject(std::optional<Object_ptr> value) : value(std::move(value)) {};
	void accept(ObjectVisitor& visitor);
};

using ScalarObject_ptr = OBJECTSYSTEM_API std::shared_ptr<ScalarObject>;
using CompositeObject_ptr = OBJECTSYSTEM_API std::shared_ptr<CompositeObject>;
using OptionalObject_ptr = OBJECTSYSTEM_API std::shared_ptr<OptionalObject>;

// Scalar Objects

struct OBJECTSYSTEM_API NumberObject : public ScalarObject, public std::enable_shared_from_this<NumberObject>
{
	double value;
	NumberObject(double value) : value(value) {};
	void accept(ObjectVisitor& visitor);
};

struct OBJECTSYSTEM_API StringObject : public ScalarObject, public std::enable_shared_from_this<StringObject>
{
	std::string value;
	StringObject(std::string value) : value(value) {};
	void accept(ObjectVisitor& visitor);
};

struct OBJECTSYSTEM_API BooleanObject : public ScalarObject, public std::enable_shared_from_this<BooleanObject>
{
	bool value;
	BooleanObject(bool value) : value(value) {};
	void accept(ObjectVisitor& visitor);
};

// Composite Object

struct OBJECTSYSTEM_API VectorObject : public CompositeObject, public std::enable_shared_from_this<VectorObject>
{
	std::vector<Object_ptr> values;

	VectorObject() {};
	void add(Object_ptr value);
	void accept(ObjectVisitor& visitor);
};

struct OBJECTSYSTEM_API UDTObject : public CompositeObject, public std::enable_shared_from_this<UDTObject>
{
	std::map<std::string, Object_ptr> pairs;

	UDTObject() {};
	void add(std::string key, Object_ptr value);
	void accept(ObjectVisitor& visitor);
};

using NumberObject_ptr = OBJECTSYSTEM_API std::shared_ptr<NumberObject>;
using StringObject_ptr = OBJECTSYSTEM_API std::shared_ptr<StringObject>;
using BooleanObject_ptr = OBJECTSYSTEM_API std::shared_ptr<BooleanObject>;
using VectorObject_ptr = OBJECTSYSTEM_API std::shared_ptr<VectorObject>;
using UDTObject_ptr = OBJECTSYSTEM_API std::shared_ptr<UDTObject>;

OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const NumberObject_ptr obj);
OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const StringObject_ptr obj);
OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const BooleanObject_ptr obj);
OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const VectorObject_ptr obj);
OBJECTSYSTEM_API std::ostream& operator<<(std::ostream& os, const UDTObject_ptr obj);

// Visitor

class OBJECTSYSTEM_API ObjectVisitor
{
public:
	virtual void visit(OptionalObject_ptr obj) = 0;

	virtual void visit(NumberObject_ptr obj) = 0;
	virtual void visit(StringObject_ptr obj) = 0;
	virtual void visit(BooleanObject_ptr obj) = 0;
	virtual void visit(VectorObject_ptr obj) = 0;
	virtual void visit(UDTObject_ptr obj) = 0;
};

class OBJECTSYSTEM_API ObjectPrinterVisitor
{
public:
	void visit(OptionalObject_ptr obj);

	void visit(NumberObject_ptr obj);
	void visit(StringObject_ptr obj);
	void visit(BooleanObject_ptr obj);
	void visit(VectorObject_ptr obj);
	void visit(UDTObject_ptr obj);
};
