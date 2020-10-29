#pragma once

#include "Type.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <optional>
#include <utility>

struct VariableSymbol;
struct CallableSymbol;
struct EnumSymbol;
struct ClassSymbol;
struct InterfaceSymbol;
struct AliasSymbol;

using Symbol = std::variant<
	std::monostate,
	VariableSymbol,
	CallableSymbol,
	EnumSymbol,
	ClassSymbol,
	InterfaceSymbol,
	AliasSymbol
>;

using Symbol_ptr = std::shared_ptr<Symbol>;
using NameTypePairs = std::vector<std::pair<std::wstring, Type_ptr>>;
using StringVector = std::vector<std::wstring>;

using VariableSymbol_ptr = std::shared_ptr<VariableSymbol>;
using CallableSymbol_ptr = std::shared_ptr<CallableSymbol>;
using EnumSymbol_ptr = std::shared_ptr<EnumSymbol>;
using ClassSymbol_ptr = std::shared_ptr<ClassSymbol>;
using InterfaceSymbol_ptr = std::shared_ptr<InterfaceSymbol>;
using AliasSymbol_ptr = std::shared_ptr<AliasSymbol>;

struct SymbolBase
{
	std::wstring name;
	Type_ptr type;
	bool is_public;

	SymbolBase(std::wstring name, bool is_public, Type_ptr type)
		: name(name), is_public(is_public), type(type) {};
};

struct VariableSymbol : public SymbolBase
{
	bool is_mutable;

	VariableSymbol(std::wstring name, bool is_public, bool is_mutable, Type_ptr type)
		: SymbolBase(name, is_public, type), is_mutable(is_mutable) {};
};

struct CallableSymbol : public SymbolBase
{
	StringVector arguments;
	Type_ptr type;

	CallableSymbol(std::wstring name, bool is_public, StringVector arguments, Type_ptr type)
		: SymbolBase(name, is_public, type), arguments(arguments), type(type) {};
};

struct EnumSymbol : public SymbolBase
{
	StringVector members;

	EnumSymbol(std::wstring name, bool is_public, StringVector members, Type_ptr type)
		: SymbolBase(name, is_public, type), members(members) {};
};

struct ClassSymbol : public SymbolBase
{
	StringVector interfaces;
	StringVector base_types;

	std::map<std::wstring, Type_ptr> field_types;
	std::map<std::wstring, CallableSymbol_ptr> method_symbols;

	StringVector public_members;

	ClassSymbol(std::wstring name, bool is_public, StringVector interfaces, StringVector base_types, std::map<std::wstring, Type_ptr> field_types, StringVector public_members, Type_ptr type)
		: SymbolBase(name, is_public, type), interfaces(interfaces), base_types(base_types), field_types(field_types), public_members(public_members) {};
};

struct InterfaceSymbol : public SymbolBase
{
	StringVector interfaces;
	StringVector base_types;

	std::map<std::wstring, Type_ptr> member_types;
	StringVector public_members;

	InterfaceSymbol(std::wstring name, bool is_public, StringVector interfaces, StringVector base_types, std::map<std::wstring, Type_ptr> member_types, StringVector public_members, Type_ptr type)
		: SymbolBase(name, is_public, type), interfaces(interfaces), base_types(base_types), member_types(member_types), public_members(public_members) {};
};

struct AliasSymbol : public SymbolBase
{
	AliasSymbol(std::wstring name, bool is_public, Type_ptr type)
		: SymbolBase(name, is_public, type) {};
};
