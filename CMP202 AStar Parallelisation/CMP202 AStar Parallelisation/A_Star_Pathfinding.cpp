#include "A_Star_Pathfinding.h"
#include <thread>

#pragma region BASE_ASTAR


void Base_A_Star_Pathfinding::SetUpStartAndEndNodes(Vector2<int> startPos, Vector2<int> endPos)
{
	Node* endNode = nullptr;
	Node* startNode = nullptr;

	//Get nodes from rooms based on position
	for (auto rm : rooms)
	{
		if (IsNodeInRoom(*rm, startPos))
		{
			 startNode = FindNodeInRoom(startPos, rm);
		}
		if (IsNodeInRoom(*rm, endPos))
		{
			endNode = FindNodeInRoom(endPos, rm);
		}
	}

	//Set up startnode
	startNode->SetGCost(0); startNode->SetParent(nullptr);
	startNode->SetHCost(floorf((endPos.x - startPos.x) + (endPos.y - startPos.y))); startNode->SetFCost();

	root = startNode;
	target = endNode;
}

/// <summary>
/// Finds and returns the node in the room given, assuming the coordinates match. Will shift as necessary for any position not comforming to the node size
/// </summary>
/// <param name="pos"></param>
/// <param name="rm"></param>
/// <returns></returns>
Node* Base_A_Star_Pathfinding::FindNodeInRoom(Vector2<int> pos, Room* rm)
{
	//Trying to get the reference of the room position back to 0
	Vector2<int> balance = rm->GetLowestCoord();
	Vector2<int> tempPos = pos;
	tempPos -= balance;

	int x, y;
	x = 0;
	y = 0;
	x = tempPos.x; y = tempPos.y;
	return &rm->nodes[x][y];

	//try
	//{
	//	//return &rm->nodes[299][299]; //This does not catch

	//}
	//catch (const std::exception&)
	//{
	//}
}


void Base_A_Star_Pathfinding::PrintRoute()
{
	Node* cn; cn = target;
	while (cn->GetParent() != nullptr)
	{
		cn->nodeType = Path;
		cn = cn->GetParent();
	}

}

///////Acknowledgment: The IsNodeInRoom functions are based on the CMP105 project - collision detection

//Collision detection code for finding if node is within certain bounds
bool Base_A_Star_Pathfinding::IsNodeInRoom(const RoomStruct& nm, const Node& target)
{
	if (nm.GetHighestCoord().x < target.position.x) { return false; }
	if (nm.GetLowestCoord().x > target.position.x) { return false; }
	if (nm.GetHighestCoord().y < target.position.y) { return false; }
	if (nm.GetLowestCoord().y > target.position.y) { return false; }

	return true;
}

//Collision detection code for finding if position is within certain bounds
bool Base_A_Star_Pathfinding::IsNodeInRoom(const RoomStruct& nm, const Vector2<int> position)
{
	if (nm.GetHighestCoord().x < position.x) { return false; }
	if (nm.GetLowestCoord().x > position.x) { return false; }
	if (nm.GetHighestCoord().y < position.y) { return false; }
	if (nm.GetLowestCoord().y > position.y) { return false; }

	return true;
}
#pragma endregion


#pragma region SEGMENTED

/// <summary>
/// Specific to segmented variant
/// </summary>
void A_Star_Pathfinding_Defined_Segmented::AStarAlgorithm()
{
	Node* current = root;
	Room* roomCheck = currentRoom;

	//While we're not in the same room, search for a path towards the room where our target node is in
	if(!IsNodeInRoom(*currentRoom, *target)) {

		std::queue<Node*> tempTargets = FindRouteNodePaths(BruteForcePathfindMaps());//temporary target nodes that join rooms together towards main target node
		//Pathfind to each target node until we run out(by that point we should be in the same room
		while (!tempTargets.empty())
		{
			Node* tempTargetNode = tempTargets.front();
			tempTargetNode = tempTargets.front();

			if (DefaultAStar(current, tempTargetNode))
			{
				current = tempTargetNode;
				if (tempTargetNode->nodeType == Routes)
				{
					TeleportNode* tpNode = static_cast<TeleportNode*>(tempTargetNode);
					tpNode->nodeToTeleportTo->SetParent(tpNode);
					current = tpNode->nodeToTeleportTo;

				}

				tempTargets.pop();
			}
			else {
				//We could not find a path towards the target node
				return;
			}
		}
	}
	//Search normally towards the target node
	if (DefaultAStar(current, target))
	{
		return;
		
	}
	std::cout << "Seg Nada";
	return;//Failed
}

std::stack<RoomStruct*> A_Star_Pathfinding_Defined_Segmented::BruteForcePathfindMaps()
{
	std::stack<RoomStruct*> mapRoute;//Rooms we need to go through
	RoomStruct* currentRoomToSearch = currentRoom;//Our current room that we're in

	std::set<RoomStruct*> open; open.insert(currentRoom);
	std::set<RoomStruct*> closed;
	//Brute force searching of all rooms
	while (open.size() != 0)
	{
		currentRoomToSearch = *open.begin();//We have search all neighbours of previous room, get next room
		open.erase(open.begin());
		closed.insert(currentRoomToSearch);
		//Search all neighbouring rooms
		for (auto neighbour : currentRoomToSearch->GetNeighbouringRooms())
		{
			if (closed.find(neighbour) != closed.end())
			{
				continue;
			}
			neighbour->SetParentRoom(currentRoomToSearch);
			if (IsNodeInRoom(*neighbour, *target))//If room is our target, start pushing path into mapRoute//otherwise add it to the open set
			{

				RoomStruct* route = neighbour;
				while (route->GetParentRoom() != nullptr)//Grab parent room repeatedly to push into routes
				{
					mapRoute.push(route);
					route = route->GetParentRoom();
				}
				mapRoute.push(route);
				return mapRoute;
			};

			open.insert(neighbour);
		}
	}
	return mapRoute;

}

std::queue<Node*> A_Star_Pathfinding_Defined_Segmented::FindRouteNodePaths(std::stack<RoomStruct*> mapRoute)
{

	std::queue<Node*> temporaryTargets;//target nodes to travel to, on the way towards main end node
	while (!mapRoute.empty())
	{
		Node* rn = FindRouteNode(mapRoute);
		if (rn == nullptr)
		{
			//We have not foudn a path
			break;
		}
		temporaryTargets.push(rn);
	}
	return temporaryTargets;
}
Node* A_Star_Pathfinding_Defined_Segmented::FindRouteNode(std::stack<RoomStruct*>& mapRoute)
{

	RoomStruct* map = mapRoute.top();
	mapRoute.pop();
	if (mapRoute.size() == 0)
	{
		return nullptr;
	}
	//The temporaryTarget nodes we will be pushing are not the route nodes but the nodes that lead to the next room, that is neighbouring that route node.
	for (auto routeNode : map->GetRouteNodes())
	{
			Node* nodeInNextRoom = routeNode->nodeToTeleportTo;
			if (nodeInNextRoom != nullptr)
			{
				if (IsNodeInRoom(*mapRoute.top(), *nodeInNextRoom))//If the routeNode's teleporting node is in the next room, return the routeNode
				{
					return routeNode;
				}
			}

	}
	return nullptr;
}
/// <summary>
/// Designed to be as modular in use as possible
/// </summary>
/// <param name="startNode"></param>
/// <param name="endNode"></param>
/// <returns></returns>
bool A_Star_Pathfinding_Defined_Segmented::DefaultAStar(Node* startNode, Node* endNode)
{
	std::set<Node*, ReverseComparator> open;
	std::set<Node*> closed;
	open.insert(startNode);

	iterations = 0;
	while (open.size() != 0 && iterations <= 1000000)
	{

		//Find lowest fCost Open Node
		Node* current = *open.begin();
		//std::cout << current->position;
		//If we found end, stop pathfinding
		//int o = current->DistanceFromM(endNode.position);//debugging
		//if (current->DistanceFromE(endNode->position) < 1)//TODO
		//{
		//	return true;
		//}
		std::set<Node*>::iterator it = open.begin();//For some reason, it won't erase some points, so we need to point to the first one to erase instead of erasing the specific node

		//Restructure the node collections
		open.erase(it);
		closed.insert(current);

		//Neighbours
		//current->GenerateNeighbours(nodeSize);
		CheckNeighbours(current, endNode, &open, &closed);

		iterations++;
	}
	return false;
}



void A_Star_Pathfinding_Defined_Segmented::CheckNeighbours(Node* current, Node* targetNode, std::set<Node*, ReverseComparator>* open, std::set<Node*>* closed)
{
	//For all neighbours : categorise them
	for (auto neighbour : current->neighbours)
	{
		//Parent node
		if (neighbour == nullptr || neighbour->nodeType == Obstacle || closed->find(neighbour) != closed->end())
		{
			continue;
		}

		if (neighbour->position == targetNode->position)
		{
			targetNode->SetParent(current);
			targetNode->SetFCost(0);
		}

		int newGCost = current->GetGCost() + 1;
		bool inToSearch = open->find(neighbour) == open->end();
		//If neighbour is in either open, and the new path is better, recalculate
		if (!inToSearch && neighbour->GetGCost() > newGCost)
		{
			neighbour->SetGCost(newGCost);
			neighbour->SetFCost();
			neighbour->SetParent(current);
			continue;
		}
		//If neighbour is not in open, set
		if (inToSearch)
		{
			int newHCost = Node::GetDistance(*neighbour, *targetNode);
			neighbour->SetGCost(newGCost);
			neighbour->SetHCost(newHCost);
			neighbour->SetFCost();
			neighbour->SetParent(current);
			open->insert(neighbour);

		}

	}
}

#pragma endregion



