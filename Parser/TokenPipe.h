#pragma once

#include <vector>
#include <memory>
#include <optional>
#include "Token.h"

class TokenPipe
{
	std::vector<Token_ptr> tokens;
	int index;

public:
	TokenPipe(std::vector<Token_ptr> tokens) : tokens(tokens), index(0) {};

	// Token Functions

	std::optional<Token_ptr> current() const;
	std::optional<Token_ptr> current(std::vector<WTokenType> ignorables);

	std::optional<Token_ptr> optional(WTokenType token_type);
	std::optional<Token_ptr> optional(WTokenType token_type, std::vector<WTokenType> ignorables);

	Token_ptr required(WTokenType token_type);
	Token_ptr required(WTokenType token_type, std::vector<WTokenType> ignorables);

	void expect(WTokenType token_type);
	void expect(WTokenType token_type, std::vector<WTokenType> ignorables);

	void expect_indent(const int expected_indent);
	bool has_indent(const int expected_indent);

	bool has_indent_and_followed_by(const int expected_indent, WTokenType token);

	void ignore(WTokenType ignorable);
	void ignore(std::vector<WTokenType> ignorables);

	// Utils

	int get_current_index() const;

	void advance_pointer();
	void retreat_pointer();

	size_t get_size() const;
};

using TokenPipe_ptr = std::shared_ptr<TokenPipe>;
