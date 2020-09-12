#pragma once

enum class Precedence : int
{
	ASSIGNMENT = 1,
	TYPE_PATTERN, // ? :
	LOGICAL, // and or
	EQUALITY, // == !=
	COMPARISON, // < > <= >= in typeof
	TERM, // + -
	PRODUCT, // * / %
	EXPONENT, // ^
	PREFIX,
	POSTFIX,
	CALL // call, member access
};
