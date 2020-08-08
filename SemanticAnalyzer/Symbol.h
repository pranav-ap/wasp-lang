#pragma once

#include "Type_System.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <optional>
#include <utility>

struct VariableSymbol;
struct FunctionSymbol;
struct EnumSymbol;
struct UDTSymbol;
struct AliasSymbol;

using Symbol = std::variant<
	std::monostate,
	VariableSymbol,
	FunctionSymbol,
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
	bool is_public;

	SymbolBase(std::wstring name, bool is_public)
		: name(name), is_public(is_public) {};
};

struct VariableSymbol : public SymbolBase
{
	bool is_mutable;
	Type_ptr type;

	VariableSymbol(std::wstring name, bool is_public, bool is_mutable, Type_ptr type)
		: SymbolBase(name, is_public), is_mutable(is_mutable), type(std::move(type)) {};
};

struct FunctionSymbol : public SymbolBase
{
	NameTypePairs argument_types;
	std::optional<Type_ptr> return_type;

	FunctionSymbol(std::wstring name, bool is_public, NameTypePairs argument_types, std::optional<Type_ptr> return_type)
		: SymbolBase(name, is_public), argument_types(argument_types), return_type(return_type) {};
};

struct EnumSymbol : public SymbolBase
{
	StringVector members;

	EnumSymbol(std::wstring name, bool is_public, StringVector members)
		: SymbolBase(name, is_public), members(members) {};
};

struct UDTSymbol : public SymbolBase
{
	std::map<std::wstring, Type_ptr> member_types;
	std::map<std::wstring, bool> is_public_member;

	UDTSymbol(std::wstring name, bool is_public, std::map<std::wstring, Type_ptr> member_types, std::map<std::wstring, bool> is_public_member)
		: SymbolBase(name, is_public), member_types(member_types), is_public_member(is_public_member) {};
};

struct AliasSymbol : public SymbolBase
{
	Type_ptr type;

	AliasSymbol(std::wstring name, bool is_public, Type_ptr type)
		: SymbolBase(name, is_public), type(type) {};
};

using VariableSymbol_ptr = std::shared_ptr<VariableSymbol>;
using FunctionSymbol_ptr = std::shared_ptr<FunctionSymbol>;
using EnumSymbol_ptr = std::shared_ptr<EnumSymbol>;
using UDTSymbol_ptr = std::shared_ptr<UDTSymbol>;
using AliasSymbol_ptr = std::shared_ptr<AliasSymbol>;
