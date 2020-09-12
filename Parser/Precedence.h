#pragma once

enum class Precedence : int
{
	ASSIGNMENT = 1,
	TYPE_PATTERN, // ? :
	OR,
	AND,
	EQUALITY, // == !=
	COMPARISON, // < > <= >= in is
	TERM, // + -
	PRODUCT, // * / %
	EXPONENT, // ^
	PREFIX, // + - ! typeof
	POSTFIX,
	CALL // call() . new ?.
};
