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

struct NumberObject;
struct StringObject;
struct BooleanObject;
struct ListObject;
struct TupleObject;
struct MapObject;
struct EnumMemberObject;
struct VariantObject;
struct ReturnObject;
struct ErrorObject;
struct BreakObject;
struct ContinueObject;
struct BuiltInsObject;
struct NoneObject;
struct FunctionObject;
struct GeneratorObject;

using Object = OBJECTSYSTEM_API std::variant<
	std::monostate,
	// Scalar Objects
	NumberObject, StringObject, BooleanObject,
	// Composite Objects
	ListObject, TupleObject,
	EnumMemberObject,
	MapObject,
	VariantObject,
	// Action Objects
	ReturnObject, ErrorObject,
	BreakObject, ContinueObject,
	BuiltInsObject,
	// Other
	NoneObject,
	FunctionObject,
	GeneratorObject
>;

using Object_ptr = OBJECTSYSTEM_API std::shared_ptr<Object>;

// Defining Objects

struct OBJECTSYSTEM_API BaseObject
{
};

struct OBJECTSYSTEM_API ScalarObject : public BaseObject
{
};

struct OBJECTSYSTEM_API CompositeObject : public BaseObject
{
};

struct OBJECTSYSTEM_API ActionObject : public BaseObject
{
};

struct OBJECTSYSTEM_API NoneObject : public BaseObject
{
};

struct OBJECTSYSTEM_API FunctionObject : public BaseObject
{
	std::vector<std::byte> instructions;
	int parameter_count;

	FunctionObject() : parameter_count(0) {};
	FunctionObject(std::vector<std::byte> instructions, int parameter_count)
		: instructions(instructions), parameter_count(parameter_count) {};
};

struct OBJECTSYSTEM_API GeneratorObject : public BaseObject
{
	std::vector<std::byte> instructions;
	int parameter_count;

	GeneratorObject() : parameter_count(0) {};
	GeneratorObject(std::vector<std::byte> instructions, int parameter_count)
		: instructions(instructions), parameter_count(parameter_count) {};
};

// Scalar Objects

struct OBJECTSYSTEM_API NumberObject : public ScalarObject
{
	double value;
	NumberObject(double value) : value(value) {};
};

struct OBJECTSYSTEM_API StringObject : public ScalarObject
{
	std::wstring value;
	StringObject(std::wstring value) : value(value) {};
};

struct OBJECTSYSTEM_API BooleanObject : public ScalarObject
{
	bool value;
	BooleanObject(bool value) : value(value) {};
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

	Object_ptr get(Object_ptr index);
	Object_ptr set(Object_ptr index, Object_ptr value);

	void clear();
	bool is_empty();
	int get_length();
};

struct OBJECTSYSTEM_API TupleObject : public CompositeObject
{
	std::vector<Object_ptr> values;

	TupleObject(std::vector<Object_ptr> values) : values(values) {};

	Object_ptr get(Object_ptr index);
	Object_ptr set(Object_ptr index, Object_ptr value);
	Object_ptr set(std::vector<Object_ptr> values);

	int get_length();
};

struct OBJECTSYSTEM_API MapObject : public CompositeObject
{
	std::map<Object_ptr, Object_ptr> pairs;

	MapObject() {};

	Object_ptr insert(Object_ptr key, Object_ptr value);
	Object_ptr get_pair(Object_ptr key);
	Object_ptr get(Object_ptr key);
	Object_ptr set(Object_ptr key, Object_ptr value);

	int get_size();
};

struct OBJECTSYSTEM_API EnumMemberObject : public CompositeObject
{
	std::wstring enum_name;
	std::vector<std::wstring> member_names;

	EnumMemberObject(std::wstring enum_name, std::vector<std::wstring> member_names)
		: enum_name(enum_name), member_names(member_names) {};
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
	std::wstring message;

	ErrorObject()
		: message(L"") {};
	ErrorObject(std::wstring message)
		: message(message) {};
};
