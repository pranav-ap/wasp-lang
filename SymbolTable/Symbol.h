#pragma once

#ifdef SYMBOLTABLE_EXPORTS
#define SYMBOLTABLE_API __declspec(dllexport)
#else
#define SYMBOLTABLE_API __declspec(dllimport)
#endif

#include "Type_System.h"
#include "Statement.h"
#include <map>
#include <set>
#include <memory>
#include <string>
#include <variant>

class VariableSymbol;
class FunctionSymbol;
class EnumSymbol;
class UDTSymbol;
class AliasSymbol;

using Symbol = SYMBOLTABLE_API std::variant<
	std::monostate,
	VariableSymbol,
	FunctionSymbol,
	EnumSymbol,
	UDTSymbol,
	AliasSymbol
>;

using Symbol_ptr = SYMBOLTABLE_API std::shared_ptr<Symbol>;

class SymbolBase
{
public:
	std::wstring name;
	bool is_public;

	SymbolBase(std::wstring name, bool is_public) : name(name), is_public(is_public) {};
};

class SYMBOLTABLE_API VariableSymbol : public SymbolBase
{
public:
	bool is_mutable;
	Type_ptr type;

	VariableSymbol(std::wstring name, bool is_public, bool is_mutable, Type_ptr type)
		: SymbolBase(name, is_public), is_mutable(is_mutable), type(std::move(type)) {};
};

class SYMBOLTABLE_API FunctionSymbol : public SymbolBase
{
public:
	std::vector<std::pair<std::wstring, Type_ptr>> argument_types;
	std::optional<Type_ptr> return_type;
	Block block;

	FunctionSymbol(std::wstring name, bool is_public, std::vector<std::pair<std::wstring, Type_ptr>> argument_types, std::optional<Type_ptr> return_type, Block block)
		: SymbolBase(name, is_public), argument_types(argument_types), return_type(return_type), block(block) {};
};

class SYMBOLTABLE_API EnumSymbol : public SymbolBase
{
public:
	std::vector<std::wstring> members;

	EnumSymbol(std::wstring name, bool is_public, std::vector<std::wstring> members)
		: SymbolBase(name, is_public), members(members) {};
};

class SYMBOLTABLE_API UDTSymbol : public SymbolBase
{
public:
	std::map<std::wstring, Type_ptr> member_types;
	std::map<std::wstring, bool> is_public_member;

	UDTSymbol(std::wstring name, bool is_public, std::map<std::wstring, Type_ptr> member_types, std::map<std::wstring, bool> is_public_member)
		: SymbolBase(name, is_public), member_types(member_types), is_public_member(is_public_member) {};
};

class SYMBOLTABLE_API AliasSymbol : public SymbolBase
{
public:
	Type_ptr type;

	AliasSymbol(std::wstring name, bool is_public, Type_ptr type)
		: SymbolBase(name, is_public), type(type) {};
};

using VariableSymbol_ptr = SYMBOLTABLE_API std::shared_ptr<VariableSymbol>;
using FunctionSymbol_ptr = SYMBOLTABLE_API std::shared_ptr<FunctionSymbol>;
using EnumSymbol_ptr = SYMBOLTABLE_API std::shared_ptr<EnumSymbol>;
using UDTSymbol_ptr = SYMBOLTABLE_API std::shared_ptr<UDTSymbol>;
using AliasSymbol_ptr = SYMBOLTABLE_API std::shared_ptr<AliasSymbol>;
