#include "Token.h"

using namespace std;

// Constructors

NumberLiteral::NumberLiteral(double value, string pos)
{
	number = value;
	position = pos;
}

StringLiteral::StringLiteral(string value, string pos)
{
	string_literal = value;
	position = pos;
}

BooleanLiteral::BooleanLiteral(bool value, string pos)
{
	truth_value = value;
	position = pos;
}

Keyword::Keyword(KeywordType value, string pos)
{
	keyword = value;
	position = pos;
}

Punctuation::Punctuation(PunctuationType value, string pos)
{
	punc = value;
	position = pos;
}

Identifier::Identifier(string value, string pos)
{
	name = value;
	position = pos;
}

FunctionIdentifier::FunctionIdentifier(string value, string pos)
{
	name = value;
	position = pos;
}

Unknown::Unknown(string value, string pos)
{
	word = value;
	position = pos;
}

// Print Out Tokens

void Token::print()
{
	cout << "Token : " << position << endl;
}

void NumberLiteral::print()
{
	cout << "NumberLiteral : " << number << " - " << position << endl;
}

void StringLiteral::print()
{
	cout << "StringLiteral : " << string_literal << " - " << position << endl;
}

void BooleanLiteral::print()
{
	cout << "BooleanLiteral : " << truth_value << " - " << position << endl;
}

void Identifier::print()
{
	cout << "Identifier : " << name << " - " << position << endl;
}

void FunctionIdentifier::print()
{
	cout << "FunctionIdentifier : " << name << " - " << position << endl;
}

void Keyword::print()
{
	switch (keyword)
	{
	case IF:
	{
		cout << "Keyword : IF - " << position << endl;
		break;
	}

	case ELSE:
	{
		cout << "Keyword : ELSE - " << position << endl;
		break;
	}

	case AND:
	{
		cout << "Keyword : AND - " << position << endl;
		break;
	}

	case OR:
	{
		cout << "Keyword : OR - " << position << endl;
		break;
	}

	case LET:
	{
		cout << "Keyword : LET - " << position << endl;
		break;
	}

	case CONST:
	{
		cout << "Keyword : CONST - " << position << endl;
		break;
	}

	case LOOP:
	{
		cout << "Keyword : LOOP - " << position << endl;
		break;
	}

	case FOR:
	{
		cout << "Keyword : FOR - " << position << endl;
		break;
	}

	case BREAK:
	{
		cout << "Keyword : BREAK - " << position << endl;
		break;
	}

	case CONTINUE:
	{
		cout << "Keyword : CONTINUE - " << position << endl;
		break;
	}

	case FN:
	{
		cout << "Keyword : FN - " << position << endl;
		break;
	}

	case RETURN:
	{
		cout << "Keyword : RETURN - " << position << endl;
		break;
	}

	case NUM:
	{
		cout << "Keyword : NUM - " << position << endl;
		break;
	}

	case STR:
	{
		cout << "Keyword : STR - " << position << endl;
		break;
	}

	case BOOL:
	{
		cout << "Keyword : BOOL - " << position << endl;
		break;
	}

	case ENUM:
	{
		cout << "Keyword : ENUM - " << position << endl;
		break;
	}

	case TYPE:
	{
		cout << "Keyword : TYPE - " << position << endl;
		break;
	}

	case OPT:
	{
		cout << "Keyword : OPT - " << position << endl;
		break;
	}

	case TRUE:
	{
		cout << "Keyword : TRUE - " << position << endl;
		break;
	}

	case FALSE:
	{
		cout << "Keyword : FALSE - " << position << endl;
		break;
	}

	case IMPORT:
	{
		cout << "Keyword : IMPORT - " << position << endl;
		break;
	}

	case FROM:
	{
		cout << "Keyword : FROM - " << position << endl;
		break;
	}

	case PUB:
	{
		cout << "Keyword : PUB - " << position << endl;
		break;
	}

	case PURE:
	{
		cout << "Keyword : PURE - " << position << endl;
		break;
	}

	default:
		break;
	}
}

void Punctuation::print()
{
	switch (punc)
	{
	case OPEN_PARENTHESIS: {
		cout << "Punctuation : OPEN_PARENTHESIS - " << position << endl;
		break;
	}
	case CLOSE_PARENTHESIS: {
		cout << "Punctuation : CLOSE_PARENTHESIS - " << position << endl;
		break;
	}
	case OPEN_CURLY_BRACE: {
		cout << "Punctuation : OPEN_CURLY_BRACE - " << position << endl;
		break;
	}
	case CLOSE_CURLY_BRACE: {
		cout << "Punctuation : CLOSE_CURLY_BRACE - " << position << endl;
		break;
	}
	case OPEN_BRACKET: {
		cout << "Punctuation : OPEN_BRACKET - " << position << endl;
		break;
	}
	case CLOSE_BRACKET: {
		cout << "Punctuation : CLOSE_BRACKET - " << position << endl;
		break;
	}
	case BACKWARD_SLASH: {
		cout << "Punctuation : BACKWARD_SLASH - " << position << endl;
		break;
	}
	case SEMICOLON: {
		cout << "Punctuation : SEMICOLON - " << position << endl;
		break;
	}
	case COMMA: {
		cout << "Punctuation : COMMA - " << position << endl;
		break;
	}
	case COLON: {
		cout << "Punctuation : COLON - " << position << endl;
		break;
	}
	case BAR: {
		cout << "Punctuation : BAR - " << position << endl;
		break;
	}
	case ARROW: {
		cout << "Punctuation : ARROW - " << position << endl;
		break;
	}
	case PLUS: {
		cout << "Punctuation : PLUS - " << position << endl;
		break;
	}
	case PLUS_EQUAL: {
		cout << "Punctuation : PLUS_EQUAL - " << position << endl;
		break;
	}
	case MINUS: {
		cout << "Punctuation : MINUS - " << position << endl;
		break;
	}
	case MINUS_EQUAL: {
		cout << "Punctuation : MINUS_EQUAL - " << position << endl;
		break;
	}
	case STAR: {
		cout << "Punctuation : STAR - " << position << endl;
		break;
	}
	case STAR_EQUAL: {
		cout << "Punctuation : STAR_EQUAL - " << position << endl;
		break;
	}
	case DIVISION: {
		cout << "Punctuation : DIVISION - " << position << endl;
		break;
	}
	case DIVISION_EQUAL: {
		cout << "Punctuation : DIVISION_EQUAL - " << position << endl;
		break;
	}
	case REMINDER: {
		cout << "Punctuation : REMINDER - " << position << endl;
		break;
	}
	case REMINDER_EQUAL: {
		cout << "Punctuation : REMINDER_EQUAL - " << position << endl;
		break;
	}
	case POWER: {
		cout << "Punctuation : POWER - " << position << endl;
		break;
	}
	case POWER_EQUAL: {
		cout << "Punctuation : POWER_EQUAL - " << position << endl;
		break;
	}
	case EQUAL: {
		cout << "Punctuation : EQUAL - " << position << endl;
		break;
	}
	case EQUAL_EQUAL: {
		cout << "Punctuation : EQUAL_EQUAL - " << position << endl;
		break;
	}
	case BANG: {
		cout << "Punctuation : BANG - " << position << endl;
		break;
	}
	case BANG_EQUAL: {
		cout << "Punctuation : BANG_EQUAL - " << position << endl;
		break;
	}
	case LESSER_THAN: {
		cout << "Punctuation : LESSER_THAN - " << position << endl;
		break;
	}
	case LESSER_THAN_EQUAL: {
		cout << "Punctuation : LESSER_THAN_EQUAL - " << position << endl;
		break;
	}
	case GREATER_THAN: {
		cout << "Punctuation : GREATER_THAN - " << position << endl;
		break;
	}
	case GREATER_THAN_EQUAL: {
		cout << "Punctuation : GREATER_THAN_EQUAL - " << position << endl;
		break;
	}
	default:
		break;
	}
}

void Unknown::print()
{
	cout << "Unknown : " << word << " - " << position << endl;
}

// UTILS

bool Keyword::type_is(KeywordType keyword_type)
{
	return keyword == keyword_type;
}

bool Punctuation::type_is(PunctuationType punctuation_type)
{
	return punc == punctuation_type;
}