#pragma once
#include "ObjectSystem.h"
#include <memory>
#include <string>

struct Symbol
{
	int id;
	std::wstring name;

	bool is_public;
	bool is_mutable;

	Object_ptr type;

	Symbol(int id, std::wstring name, Object_ptr type, bool is_public, bool is_mutable)
		: id(id), name(name), type(type), is_public(is_public), is_mutable(is_mutable) {};
};

using Symbol_ptr = std::shared_ptr<Symbol>;
