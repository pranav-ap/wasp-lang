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
struct UDTSymbol;
struct AliasSymbol;

using Symbol = std::variant<
	std::monostate,
	VariableSymbol,
	CallableSymbol,
	EnumSymbol,
	UDTSymbol,
	AliasSymbol
>;

using Symbol_ptr = std::shared_ptr<Symbol>;
using NameTypePairs = std::vector<std::pair<std::wstring, Type_ptr>>;
using StringVector = std::vector<std::wstring>;

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
	NameTypePairs argument_types;

	CallableSymbol(std::wstring name, bool is_public, NameTypePairs argument_types, Type_ptr type)
		: SymbolBase(name, is_public, type), argument_types(argument_types) {};
};

struct EnumSymbol : public SymbolBase
{
	StringVector members;

	EnumSymbol(std::wstring name, bool is_public, StringVector members, Type_ptr type)
		: SymbolBase(name, is_public, type), members(members) {};
};

struct UDTSymbol : public SymbolBase
{
	std::map<std::wstring, Type_ptr> member_types;
	std::map<std::wstring, bool> is_public_member;

	UDTSymbol(std::wstring name, bool is_public, std::map<std::wstring, Type_ptr> member_types, std::map<std::wstring, bool> is_public_member, Type_ptr type)
		: SymbolBase(name, is_public, type), member_types(member_types), is_public_member(is_public_member) {};
};

struct AliasSymbol : public SymbolBase
{
	AliasSymbol(std::wstring name, bool is_public, Type_ptr type)
		: SymbolBase(name, is_public, type) {};
};

using VariableSymbol_ptr = std::shared_ptr<VariableSymbol>;
using CallableSymbol_ptr = std::shared_ptr<CallableSymbol>;
using EnumSymbol_ptr = std::shared_ptr<EnumSymbol>;
using UDTSymbol_ptr = std::shared_ptr<UDTSymbol>;
using AliasSymbol_ptr = std::shared_ptr<AliasSymbol>;
