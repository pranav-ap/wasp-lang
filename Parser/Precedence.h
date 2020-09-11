#pragma once

enum class Precedence : int
{
	TERNARY_CONDITION = 1, // ? :
	LOGICAL, // and or
	EQUALITY, // == !=
	COMPARISON, // < > <= >=
	TERM, // + -
	PRODUCT, // * / %
	EXPONENT, // ^
	PREFIX,
	POSTFIX,
	CALL // call, member access
};
