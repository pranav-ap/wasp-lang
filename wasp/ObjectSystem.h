#pragma once

#include <optional>
#include <string>
#include <deque>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <variant>

struct NumberObject;
struct StringObject;
struct BooleanObject;
struct ListObject;
struct TupleObject;
struct MapObject;
struct UDTObject;
struct EnumMemberObject;
struct VariantObject;
struct PassObject;
struct ReturnObject;
struct ErrorObject;
struct BreakObject;
struct ContinueObject;
struct NoneObject;

using Object = std::variant<
	std::monostate,
	// Scalar Objects
	NumberObject, StringObject, BooleanObject,
	// Composite Objects
	ListObject, TupleObject,
	MapObject, UDTObject,
	EnumMemberObject,
	VariantObject,
	// Action Objects
	PassObject, ReturnObject, ErrorObject,
	BreakObject, ContinueObject,
	// Other
	NoneObject
>;

using Object_ptr = std::shared_ptr<Object>;

// Defining Objects

struct BaseObject
{
	virtual std::string stringify() = 0;
};

struct ScalarObject : public BaseObject
{
	virtual std::string stringify() = 0;
};

struct CompositeObject : public BaseObject
{
	virtual std::string stringify() = 0;
};

struct ActionObject : public BaseObject
{
	virtual std::string stringify() = 0;
};

struct NoneObject : public BaseObject
{
	std::string stringify();
};

// Scalar Objects

struct NumberObject : public ScalarObject
{
	double value;
	NumberObject(double value) : value(value) {};
	std::string stringify();
};

struct StringObject : public ScalarObject
{
	std::string value;
	StringObject(std::string value) : value(value) {};
	std::string stringify();
};

struct BooleanObject : public ScalarObject
{
	bool value;
	BooleanObject(bool value) : value(value) {};
	std::string stringify();
};

// Composite Objects

struct ListObject : public CompositeObject
{
	std::deque<Object_ptr> values;

	ListObject() {};

	Object_ptr append(Object_ptr value);
	Object_ptr prepend(Object_ptr value);

	Object_ptr pop_back();
	Object_ptr pop_front();

	Object_ptr get(Object_ptr index);

	Object_ptr set(Object_ptr index, Object_ptr value);
	Object_ptr set(std::deque<Object_ptr> values);

	bool is_empty();
	void clear();
	int get_length();

	std::string stringify();
};

struct TupleObject : public CompositeObject
{
	std::vector<Object_ptr> values;

	TupleObject(std::vector<Object_ptr> values) : values(values) {};

	Object_ptr get(Object_ptr index);

	Object_ptr set(Object_ptr index, Object_ptr value);
	Object_ptr set(std::vector<Object_ptr> values);

	int get_length();

	std::string stringify();
};

struct MapObject : public CompositeObject
{
	std::map<Object_ptr, Object_ptr> pairs;

	MapObject() {};

	Object_ptr insert(Object_ptr key, Object_ptr value);
	Object_ptr remove(Object_ptr key);

	Object_ptr get_pair(Object_ptr key);
	Object_ptr get(Object_ptr key);
	Object_ptr set(Object_ptr key, Object_ptr value);

	int get_size();

	std::string stringify();
};

struct UDTObject : public CompositeObject
{
	std::map<Object_ptr, Object_ptr> pairs;

	UDTObject() {};

	Object_ptr get(Object_ptr key);
	Object_ptr set(Object_ptr key, Object_ptr value);

	std::string stringify();
};

struct EnumMemberObject : public CompositeObject
{
	std::string enum_name;
	std::string member_name;

	EnumMemberObject(std::string enum_name, std::string member_name)
		: enum_name(enum_name), member_name(member_name) {};

	std::string stringify();
};

struct VariantObject : public CompositeObject
{
	Object_ptr value;

	VariantObject(Object_ptr value) : value(std::move(value)) {};
	bool has_value();
	Object_ptr safe_get_value();

	std::string stringify();
};

// Action Objects

struct PassObject : public ActionObject
{
	std::string stringify();
};

struct BreakObject : public ActionObject
{
	std::string stringify();
};

struct ContinueObject : public ActionObject
{
	std::string stringify();
};

struct ReturnObject : public ActionObject
{
	std::optional<Object_ptr> value;

	ReturnObject()
		: value(std::nullopt) {};
	ReturnObject(Object_ptr value)
		: value(std::optional(std::move(value))) {};

	std::string stringify();
};

struct ErrorObject : public ActionObject
{
	std::string message;

	ErrorObject()
		: message("") {};
	ErrorObject(std::string message)
		: message(message) {};

	std::string stringify();
};

using ErrorObject_ptr = std::shared_ptr<ErrorObject>;
