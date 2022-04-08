#include "A_Star_Pathfinding.h"

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

	//In case the position is off by some bits of size
	int x, y, shift;
	x = 0;
	y = 0;
	if (tempPos.x != 0)
	{
		shift = rm->GetNodeSize() % tempPos.x;//Translate ourselves to nodes positions if we start at 30 but nodesize is 50 then we add 20;
		x = (tempPos.x + shift) / rm->GetNodeSize();
	}
	if (tempPos.y != 0)
	{
		shift = rm->GetNodeSize() % tempPos.y;
		y = (tempPos.y + shift) / rm->GetNodeSize();
	}

	return &rm->nodes[x][y];
}


void Base_A_Star_Pathfinding::PrintRoute()
{
	//Sets the route to 2 to easily identify
	Node* cn; cn = target;
	while (cn->GetParent() != nullptr)
	{
		cn->nodeType = Routes;
		cn = cn->GetParent();
	}

	//Print room map 
	bool newLine = false;
	for (auto rm : rooms)
	{
		for (int x = 0; x < rm->GetXSize(); x++)
		{
			for (int y = 0; y < rm->GetYSize(); y++)
			{
				if (newLine)
				{
					std::cout << rm->nodes[y][x].nodeType << std::endl;
					newLine = false;
				}
				else {
					std::cout << rm->nodes[y][x].nodeType << " | ";
				}
			}
			newLine = true;

		}
	}

}

///////Acknowledgment: The IsNodeInRoom functions are based on the CMP105 project - collision detection

bool Base_A_Star_Pathfinding::IsNodeInRoom(const RoomStruct& nm, const Node& target)
{
	if (nm.GetHighestCoord().x < target.position.x) { return false; }
	if (nm.GetLowestCoord().x > target.position.x) { return false; }
	if (nm.GetHighestCoord().y < target.position.y) { return false; }
	if (nm.GetLowestCoord().y > target.position.y) { return false; }

	return true;
}

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
void A_Star_Pathfinding_Defined_Segmented::FindCurrentRoom(const Vector2<int> rootPosition)
{
	for (int i = 0; i < rooms.size(); i++)
	{
		if (IsNodeInRoom(*rooms[i], rootPosition))
		{
			SetCurrentRoom(rooms[i]);
			return;
		}
	}
}

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

			if (DefaultAStar(*current, *tempTargetNode))
			{
				current = tempTargetNode;
				tempTargets.pop();
			}
			else {
				//We could not find a path towards the target node
				return;
			}
		}
	}
	//Search normally towards the target node
	if (DefaultAStar(*current, *target))
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
		//Search all neighbours of routeNode
		for (int i = 0; i < 8; i++)
		{
			Node* currentNeighbour = routeNode->neighbours[i];
			if (currentNeighbour != nullptr)
			{
				if (IsNodeInRoom(*mapRoute.top(), *currentNeighbour))
				{
					return currentNeighbour;
				}
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
bool A_Star_Pathfinding_Defined_Segmented::DefaultAStar(Node& startNode, Node& endNode)
{
	std::set<Node*, ReverseComparator> open;
	std::set<Node*> closed;
	open.insert(&startNode);

	iterations = 0;
	while (open.size() != 0 && iterations <= 1000000)
	{

		//Find lowest fCost Open Node
		Node* current = *open.begin();
		//std::cout << current->position;
		//If we found end, stop pathfinding
		//int o = current->DistanceFromM(endNode.position);//debugging
		if (current->DistanceFromM(endNode.position) < nodeSize)
		{
			return true;
		}
		std::set<Node*>::iterator it = open.begin();//For some reason, it won't erase some points, so we need to point to the first one to erase instead of erasing the specific node

		//Restructure the node collections
		open.erase(it);
		closed.insert(current);

		//Neighbours
		//current->GenerateNeighbours(nodeSize);
		CheckNeighbours(current, endNode, open, closed);

		iterations++;
	}
	return false;
}



void A_Star_Pathfinding_Defined_Segmented::CheckNeighbours(Node* current, Node& targetNode, std::set<Node*, ReverseComparator>& open, std::set<Node*>& closed)
{
	//For all neighbours : categorise them
	for (auto neighbour : current->neighbours)
	{
		//Parent node
		if (neighbour == nullptr || neighbour->nodeType == Obstacle || closed.find(neighbour) != closed.end())
		{
			continue;
		}
		//To stop a infinite parent loop from previous route node(as that isn't added into the next closed set
		if (neighbour->parentNode != nullptr)
		{
			continue;
		}

		int newGCost = current->GetGCost() + 1;
		bool inToSearch = open.find(neighbour) == open.end();
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
			int newHCost = Node::DistanceBetweenM(*neighbour, targetNode);
			neighbour->SetGCost(newGCost);
			neighbour->SetHCost(newHCost);
			neighbour->SetFCost();
			neighbour->SetParent(current);
			open.insert(neighbour);

		}

	}
}

#pragma endregion



