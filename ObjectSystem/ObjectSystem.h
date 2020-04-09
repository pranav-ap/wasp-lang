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

#define MAKE_OBJECT_VARIANT(x) std::make_shared<ObjectVariant>(x)
#define VOID std::make_shared<ObjectVariant>(ReturnObject())
#define THROW(message) std::make_shared<ObjectVariant>(ErrorObject(message))

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

struct VectorObject;
struct UDTObject;
struct EnumObject;
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
	VectorObject, UDTObject, EnumObject, OptionalObject,
	// Action Objects
	ReturnObject, BreakObject, ContinueObject, BuiltInsObject,
	ErrorObject
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
	void add(ObjectVariant_ptr value);
};

struct OBJECTSYSTEM_API UDTObject : public CompositeObject
{
	std::map<std::string, ObjectVariant_ptr> pairs;

	UDTObject() {};
	void add(std::string key, ObjectVariant_ptr value);
};

struct OBJECTSYSTEM_API EnumObject : public CompositeObject
{
	std::string enum_name;
	std::string member_name;

	EnumObject(std::string enum_name, std::string member_name)
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
	ErrorObject(std::string message) : message(message) {};
};
