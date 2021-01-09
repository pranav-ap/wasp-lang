#pragma once

#include "Token.h"
#include "TokenPipe.h"
#include "Expression.h"
#include "Precedence.h"
#include <memory>

class Parser;
using Parser_ptr = Parser*;

// Expression Parselet interfaces

class IPrefixParselet
{
public:
	virtual Expression_ptr parse(Parser_ptr parser, Token_ptr token) = 0;
};

class IInfixParselet
{
public:
	virtual Expression_ptr parse(Parser_ptr parser, Expression_ptr left, Token_ptr right) = 0;
	virtual int get_precedence() = 0;
};

using IPrefixParselet_ptr = std::shared_ptr<IPrefixParselet>;
using IInfixParselet_ptr = std::shared_ptr<IInfixParselet>;

// Expression Parselets

class IdentifierParselet : public IPrefixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
};

class LiteralParselet : public IPrefixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
};

class PrefixOperatorParselet : public IPrefixParselet
{
	int precedence;

public:
	PrefixOperatorParselet(int precedence) : precedence(precedence) {};

	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
	int get_precedence();
};

class InfixOperatorParselet : public IInfixParselet
{
	int precedence;
	bool is_right_associative;

public:
	InfixOperatorParselet(int precedence, bool is_right_associative)
		: precedence(precedence), is_right_associative(is_right_associative) {};

	Expression_ptr parse(Parser_ptr parser, Expression_ptr left, Token_ptr token);
	int get_precedence();
};

class PostfixOperatorParselet : public IInfixParselet
{
	int precedence;

public:
	PostfixOperatorParselet(int precedence) : precedence(precedence) {};
	Expression_ptr parse(Parser_ptr parser, Expression_ptr left, Token_ptr token);
	int get_precedence();
};

class GroupParselet : public IPrefixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
};

class ListParselet : public IPrefixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
};

class TupleParselet : public IPrefixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
};

class SetParselet : public IPrefixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
};

class MapParselet : public IPrefixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
};

class TernaryConditionParselet : public IPrefixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
	int get_precedence();
};

class SpreadParselet : public IPrefixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Token_ptr token);
	int get_precedence();
};

class AssignmentParselet : public IInfixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Expression_ptr left, Token_ptr token);
	int get_precedence();
};

class TypePatternParselet : public IInfixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Expression_ptr left, Token_ptr token);
	int get_precedence();
};

class DoubleColonPairParselet : public IInfixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Expression_ptr left, Token_ptr token);
	int get_precedence();
};

class CallParselet : public IInfixParselet
{
public:
	Expression_ptr parse(Parser_ptr parser, Identifier* left);
	Expression_ptr parse(Parser_ptr parser, Expression_ptr left, Token_ptr token);
	int get_precedence();
};

std::wstring concat(StringVector items, std::wstring connector);
