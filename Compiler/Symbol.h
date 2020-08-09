#pragma once

#include <memory>
#include <string>
#include <utility>

struct Symbol
{
	std::wstring name;
	int id;

	Symbol(std::wstring name, int id)
		: name(name), id(id) {};
};

using Symbol_ptr = std::shared_ptr<Symbol>;
