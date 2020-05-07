#pragma once
#include "test_utils.h"
#include "Token.h"
#include "Module.h"
#include "Statement.h"
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;
using std::make_shared;

vector<Token> prepare_to_assert(vector<Token_ptr> vector_of_token_pointers)
{
	vector<Token> vec;

	for (const auto token_ptr : vector_of_token_pointers)
	{
		token_ptr->line_num = 0;
		token_ptr->column_num = 0;

		vec.push_back(*token_ptr);
	}

	return vec;
}

vector<Token_ptr> prepare_to_assert(vector<Token> vector_of_tokens)
{
	vector<Token_ptr> vec;

	for (const auto token : vector_of_tokens)
	{
		vec.push_back(make_shared<Token>(token.value, token.type));
	}

	return vec;
}