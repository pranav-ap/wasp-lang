#pragma once

enum class Precedence : int
{
	ASSIGNMENT = 1,
	TERNARY_CONDITION,
	TYPE_PATTERN, // :
	OR,
	AND,
	EQUALITY, // == !=
	COMPARISON, // < > <= >= in is
	TERM, // + -
	PRODUCT, // * / %
	EXPONENT, // ^
	PREFIX, // + - ! typeof
	POSTFIX,
	CALL, // call() new
	MEMBER_ACCESS // . ?.
};
