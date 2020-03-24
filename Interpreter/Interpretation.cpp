#pragma once
#include "pch.h"
#include "Interpreter.h"
#include<memory>

void Interpreter::execute()
{
	//for (auto const& statement : this->mod.get_statements())
	//	this->evaluate_statement(std::move(*statement));
}

void Interpreter::evaluate_statement(Statement_ptr statement)
{
	/*visit([this](auto s) {
		using T = std::decay_t<decltype(s)>;

		if constexpr (std::is_same_v<T, ExpressionStatement>)
		{
			auto x = this->evaluate_expression(s);
		}
		}, *statement.get());*/
}

Object_ptr Interpreter::evaluate_expression(ExpressionStatement expression)
{
	return nullptr;
}