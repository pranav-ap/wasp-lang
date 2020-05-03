#pragma once
#include "Token.h"
#include <map>
#include <vector>

enum class ExpressionContext
{
	FUNCTION_CALL,
	LIST_LITERAL,
	TUPLE_LITERAL,
	UDT_LITERAL,
	MAP_LITERAL,
	PARENTHESIS,
	SEQUENCE_MEMBER_ACCESS,
	NONE
};
