
#include "node_map.h"
#include "node.h"
#include "wire.h" // BUG

NodeMap::node_ptr NodeMap::new_node(int x, int y)
{
	std::shared_ptr<Node> newNode(new Node(x, y));
	_list.append(newNode);
	return newNode;
}

NodeMap::node_ptr NodeMap::new_node(int x, int y, std::shared_ptr<Wire> w)
{
	auto newNode = new_node(x,y);
	newNode->appendConnection(std::dynamic_pointer_cast<Element>(w));
	return newNode;
}

NodeMap::node_ptr NodeMap::new_node(int x, int y, std::shared_ptr<Element> e)
{
	auto newNode = new_node(x,y);
	newNode->connect(e);
	return newNode;
}

void NodeMap::clear()
{
	_list.clear();
}

NodeMap::iterator NodeMap::find(Node* n)
{
	return _list.find(n);
}

#if 0
NodeMap::iterator NodeMap::find(int x, int y)
{
	return _list.find(x, y);
}
#endif
