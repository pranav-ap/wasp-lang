#pragma once

#include "ObjectSystem.h"
#include <string>
#include <vector>

namespace io
{
	Object_ptr echo_visit(std::vector<Object_ptr> arguments);
	Object_ptr ask(std::vector<Object_ptr> arguments);
}

namespace core
{
}
