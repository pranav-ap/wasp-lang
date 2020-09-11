#pragma once

enum class Precedence : int
{
	ASSIGNMENT = 1,
	TERNARY_CONDITION, // ? :
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
