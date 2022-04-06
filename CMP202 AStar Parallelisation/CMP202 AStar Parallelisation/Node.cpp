#include "Node.h"

void Node::CalculateNodeType(const std::vector<Vector2<int>>& obsMap, int nodeSize)
{
	for (int i = 0; i < obsMap.size(); i++)
	{
		if (Vector2<int>::DistanceBetween(obsMap[i], position) < nodeSize)
		{
			nodeType = Obstacle;
			return;
		}
	}
	nodeType = Free;
}

void Node::Generate4Neighbours(int nodeSize)
{
	Vector2<int> north = position + Vector2<int>(0, 1 * nodeSize);
	Vector2<int> south = position + Vector2<int>(0, -1 * nodeSize);
	Vector2<int> east = position + Vector2<int>(1 * nodeSize, 0);
	Vector2<int> west = position + Vector2<int>(-1 * nodeSize, 0);

	//Don't make a neighbour of your parents!
	Vector2<int> parentNodePosition = position;

	if (parentNode != nullptr)
	{
		parentNodePosition = parentNode->position;
	}

	for (int i = 0; i < 4; i++)
	{
		neighbours[i] = nullptr;
	}

	if (parentNodePosition != north)
	{
		neighbours[0] = new Node(north);
	}
	if (parentNodePosition != south) {
		neighbours[1] = new Node(south);
	}
	if (parentNodePosition != east) {
		neighbours[2] = new Node(east);
	}
	if (parentNodePosition != west) {
		neighbours[3] = new Node(west);
	}
	for (int i = 0; i < 4; i++)
	{
		if (neighbours[i] != nullptr)
		{
			neighbours[i]->parentNode = this;
		}
	}
}
