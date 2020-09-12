#pragma once

enum class Precedence : int
{
	ASSIGNMENT = 1,
	TYPE_PATTERN, // ? :
	LOGICAL, // and or
	EQUALITY, // == !=
	COMPARISON, // < > <= >= in instanceof
	TERM, // + -
	PRODUCT, // * / %
	EXPONENT, // ^
	PREFIX, // + - ! typeof
	POSTFIX,
	CALL // call . new ?.
};
