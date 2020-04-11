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
#include <variant>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

struct UDTObject;
struct UDTKeyValuePairObject;

struct EnumObject;
struct EnumMemberObject;

struct VectorObject;
struct OptionalObject;

struct ReturnObject;
struct ErrorObject;
struct BreakObject;
struct ContinueObject;
struct BuiltInsObject;

// Variant Definition

using ObjectVariant = OBJECTSYSTEM_API std::variant<
	std::monostate,
	// Scalar Objects
	double, std::string, bool,
	// Composite Objects
	VectorObject, OptionalObject,
	UDTObject, UDTKeyValuePairObject,
	EnumObject, EnumMemberObject,
	// Action Objects
	ReturnObject, ErrorObject,
	BreakObject, ContinueObject,
	BuiltInsObject
>;

using ObjectVariant_ptr = OBJECTSYSTEM_API std::shared_ptr<ObjectVariant>;

// Defining Objects

struct OBJECTSYSTEM_API Object
{
};

struct OBJECTSYSTEM_API CompositeObject : public Object
{
};

struct OBJECTSYSTEM_API ActionObject : public Object
{
};

// Composite Objects

struct OBJECTSYSTEM_API VectorObject : public CompositeObject
{
	std::vector<ObjectVariant_ptr> values;

	VectorObject() {};
	ObjectVariant_ptr add(ObjectVariant_ptr value);
	ObjectVariant_ptr get_element(double index);
};

struct OBJECTSYSTEM_API UDTKeyValuePairObject : public CompositeObject
{
	std::string key;
	ObjectVariant_ptr value;

	UDTKeyValuePairObject(std::string key, ObjectVariant_ptr value)
		: key(key), value(value) {};
};

struct OBJECTSYSTEM_API UDTObject : public CompositeObject
{
	std::map<std::string, ObjectVariant_ptr> pairs;

	UDTObject() {};
	ObjectVariant_ptr add(std::string key, ObjectVariant_ptr value);
	ObjectVariant_ptr get_pair(std::string key);
	ObjectVariant_ptr get_value(std::string key);
};

struct OBJECTSYSTEM_API EnumObject : public CompositeObject
{
	std::vector<std::string> member_names;

	EnumObject(std::vector<std::string> member_names)
		: member_names(member_names) {};
};

struct OBJECTSYSTEM_API EnumMemberObject : public CompositeObject
{
	std::string enum_name;
	std::string member_name;

	EnumMemberObject(std::string enum_name, std::string member_name)
		: enum_name(enum_name), member_name(member_name) {};
};

struct OBJECTSYSTEM_API OptionalObject : public CompositeObject
{
	std::optional<ObjectVariant_ptr> value;

	OptionalObject() : value(std::nullopt) {};
	OptionalObject(std::optional<ObjectVariant_ptr> value)
		: value(std::optional<ObjectVariant_ptr>(std::move(value))) {};
};

// Action Objects

struct OBJECTSYSTEM_API ReturnObject : public ActionObject
{
	std::optional<ObjectVariant_ptr> value;

	ReturnObject() : value(std::nullopt) {};
	ReturnObject(std::optional<ObjectVariant_ptr> value)
		: value(std::optional<ObjectVariant_ptr>(std::move(value))) {};
};

struct OBJECTSYSTEM_API BreakObject : public ActionObject
{
};

struct OBJECTSYSTEM_API ContinueObject : public ActionObject
{
};

struct OBJECTSYSTEM_API BuiltInsObject : public ActionObject
{
};

struct OBJECTSYSTEM_API ErrorObject : public ActionObject
{
	std::string message;
	ErrorObject() : message("") {};
	ErrorObject(std::string message) : message(message) {};
};
