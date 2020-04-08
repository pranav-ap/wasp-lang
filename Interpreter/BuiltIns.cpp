#include "pch.h"
#include "BuiltIns.h"
#include "logger.h"

#include <iostream>
#include <string>

using std::cout;
using std::cin;
using std::endl;
using std::string;

void Core::echo(Object_ptr content)
{
	if (typeid(*content) == typeid(NumberObject))
	{
		auto number = dynamic_pointer_cast<NumberObject>(content);
		cout << number->value << endl;
	}
	else if (typeid(*content) == typeid(StringObject))
	{
		auto text = dynamic_pointer_cast<StringObject>(content);
		cout << text->value << endl;
	}
	else
	{
		FATAL("Only numbers and strings can be printed by echo");
	}
}

void Core::ask(Object_ptr text, Object_ptr variable)
{
	cout << text;

	if (typeid(*variable) == typeid(NumberObject))
	{
		auto x = dynamic_pointer_cast<NumberObject>(variable);
		cin >> x->value;
	}
	else if (typeid(*variable) == typeid(StringObject))
	{
		auto x = dynamic_pointer_cast<StringObject>(variable);
		cin >> x->value;
	}
	else
	{
		FATAL("Only numbers and strings can be read from command line");
	}
}