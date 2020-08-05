#pragma once

#ifdef SYMBOLTABLE_EXPORTS
#define SYMBOLTABLE_API __declspec(dllexport)
#else
#define SYMBOLTABLE_API __declspec(dllimport)
#endif

#include "Type_System.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <optional>

struct VariableSymbol;
struct FunctionSymbol;
struct EnumSymbol;
struct UDTSymbol;
struct AliasSymbol;

using Symbol = SYMBOLTABLE_API std::variant<
	std::monostate,
	VariableSymbol,
	FunctionSymbol,
	EnumSymbol,
	UDTSymbol,
	AliasSymbol
>;

using Symbol_ptr = SYMBOLTABLE_API std::shared_ptr<Symbol>;

struct ISymbol
{
	std::wstring name;
	bool is_public;

	ISymbol(std::wstring name, bool is_public)
		: name(name), is_public(is_public) {};
};

struct SYMBOLTABLE_API VariableSymbol : public ISymbol
{
	bool is_mutable;
	Type_ptr type;

	VariableSymbol(std::wstring name, bool is_public, bool is_mutable, Type_ptr type)
		: ISymbol(name, is_public), is_mutable(is_mutable), type(std::move(type)) {};
};

struct SYMBOLTABLE_API FunctionSymbol : public ISymbol
{
	std::vector<std::pair<std::wstring, Type_ptr>> argument_types;
	std::optional<Type_ptr> return_type;

	FunctionSymbol(std::wstring name, bool is_public, std::vector<std::pair<std::wstring, Type_ptr>> argument_types, std::optional<Type_ptr> return_type)
		: ISymbol(name, is_public), argument_types(argument_types), return_type(return_type) {};
};

struct SYMBOLTABLE_API EnumSymbol : public ISymbol
{
	std::vector<std::wstring> members;

	EnumSymbol(std::wstring name, bool is_public, std::vector<std::wstring> members)
		: ISymbol(name, is_public), members(members) {};
};

struct SYMBOLTABLE_API UDTSymbol : public ISymbol
{
	std::map<std::wstring, Type_ptr> member_types;
	std::map<std::wstring, bool> is_public_member;

	UDTSymbol(std::wstring name, bool is_public, std::map<std::wstring, Type_ptr> member_types, std::map<std::wstring, bool> is_public_member)
		: ISymbol(name, is_public), member_types(member_types), is_public_member(is_public_member) {};
};

struct SYMBOLTABLE_API AliasSymbol : public ISymbol
{
	Type_ptr type;

	AliasSymbol(std::wstring name, bool is_public, Type_ptr type)
		: ISymbol(name, is_public), type(type) {};
};

using VariableSymbol_ptr = SYMBOLTABLE_API std::shared_ptr<VariableSymbol>;
using FunctionSymbol_ptr = SYMBOLTABLE_API std::shared_ptr<FunctionSymbol>;
