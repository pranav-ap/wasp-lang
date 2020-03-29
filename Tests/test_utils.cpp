#pragma once
#include "test_utils.h"
#include "Token.h"
#include <vector>

using std::vector;

vector<Token> compose_tokens(vector<Token_ptr> vector_of_pointers)
{
	vector<Token> vec;

	for (const auto token_ptr : vector_of_pointers)
	{
		token_ptr->set_line_num(0);
		token_ptr->set_column_num(0);

		vec.push_back(*token_ptr);
	}

	return vec;
}