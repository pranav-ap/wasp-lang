#pragma once

enum class ERROR_CODE
{
	UNEXPECTED_TOKEN,
	UNEXPECTED_KEYWORD,

	EXPECTED_INDENT,
	EXPECTED_DATATYPE,

	INVALID_MAP_KEY,
	ARITY_ERROR,

	MALFORMED_EXPRESSION,
	MALFORMED_STATEMENT,

	EXPRESSION_PARSER_CONTEXT_MISMATCH
};
