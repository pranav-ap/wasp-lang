#pragma once

#ifdef OBJECTSYSTEM_EXPORTS
#define OBJECTSYSTEM_API __declspec(dllexport)
#else
#define OBJECTSYSTEM_API __declspec(dllimport)
#endif

#include <optional>
#include <string>
#include <deque>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <variant>

struct ListObject;
struct EnumMemberObject;
struct DictionaryObject;
struct TupleObject;
struct VariantObject;
struct ReturnObject;
struct ErrorObject;
struct BreakObject;
struct ContinueObject;
struct BuiltInsObject;

using Object = OBJECTSYSTEM_API std::variant<
	std::monostate,
	// Scalar Objects
	double, std::string, bool,
	// Composite Objects
	ListObject, EnumMemberObject,
	DictionaryObject, TupleObject,
	VariantObject,
	// Action Objects
	ReturnObject, ErrorObject,
	BreakObject, ContinueObject,
	BuiltInsObject
>;

using Object_ptr = OBJECTSYSTEM_API std::shared_ptr<Object>;

// Defining Objects

struct OBJECTSYSTEM_API BaseObject
{
};

struct OBJECTSYSTEM_API CompositeObject : public BaseObject
{
};

struct OBJECTSYSTEM_API ActionObject : public BaseObject
{
};

// Composite Objects

struct OBJECTSYSTEM_API ListObject : public CompositeObject
{
	std::deque<Object_ptr> values;

	ListObject() {};

	Object_ptr append(Object_ptr value);
	Object_ptr prepend(Object_ptr value);

	Object_ptr pop_back();
	Object_ptr pop_front();

	Object_ptr get(int index);
	Object_ptr set(int index, Object_ptr value);

	void clear();
	bool is_empty();
	int get_length();
};

struct OBJECTSYSTEM_API TupleObject : public CompositeObject
{
	std::vector<Object_ptr> values;

	TupleObject(std::vector<Object_ptr> values) : values(values) {};

	Object_ptr get(int index);
	Object_ptr set(int index, Object_ptr value);
	Object_ptr set(std::vector<Object_ptr> values);

	int get_length();
};

struct OBJECTSYSTEM_API DictionaryObject : public CompositeObject
{
	std::map<Object_ptr, Object_ptr> pairs;

	DictionaryObject() {};

	Object_ptr insert(Object_ptr key, Object_ptr value);
	Object_ptr get_pair(Object_ptr key);
	Object_ptr get(Object_ptr key);
	Object_ptr set(Object_ptr key, Object_ptr value);

	int get_size();
};

struct OBJECTSYSTEM_API EnumMemberObject : public CompositeObject
{
	std::string enum_name;
	std::string member_name;

	EnumMemberObject(std::string enum_name, std::string member_name)
		: enum_name(enum_name), member_name(member_name) {};
};

struct OBJECTSYSTEM_API VariantObject : public CompositeObject
{
	Object_ptr value;

	VariantObject(Object_ptr value)
		: value(std::move(value)) {};
	bool has_value();
};

// Action Objects

struct OBJECTSYSTEM_API BreakObject : public ActionObject
{
};

struct OBJECTSYSTEM_API ContinueObject : public ActionObject
{
};

struct OBJECTSYSTEM_API BuiltInsObject : public ActionObject
{
};

struct OBJECTSYSTEM_API ReturnObject : public ActionObject
{
	std::optional<Object_ptr> value;

	ReturnObject()
		: value(std::nullopt) {};
	ReturnObject(Object_ptr value)
		: value(std::optional(std::move(value))) {};
};

struct OBJECTSYSTEM_API ErrorObject : public ActionObject
{
	std::string message;

	ErrorObject()
		: message("") {};
	ErrorObject(std::string message)
		: message(message) {};
};
