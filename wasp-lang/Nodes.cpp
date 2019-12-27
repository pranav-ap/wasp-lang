#include "Nodes.h"

using namespace std;

void Module::add(shared_ptr<StatementNode> node)
{
	this->nodes.push_back(node);
}