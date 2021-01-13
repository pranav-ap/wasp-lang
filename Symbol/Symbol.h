#pragma once

#ifdef SYMBOL_EXPORTS
#define SYMBOL_API __declspec(dllexport)
#else
#define SYMBOL_API __declspec(dllimport)
#endif

#include "Objects.h"
#include <memory>
#include <string>

struct SYMBOL_API Symbol
{
	int id;
	std::wstring name;

	bool is_public;
	bool is_mutable;
	bool is_builtin;

	Object_ptr type;

	Symbol(int id, std::wstring name, Object_ptr type, bool is_public, bool is_mutable)
		: id(id), name(name), type(type), is_public(is_public), is_mutable(is_mutable), is_builtin(false) {};
};

using Symbol_ptr = SYMBOL_API std::shared_ptr<Symbol>;
