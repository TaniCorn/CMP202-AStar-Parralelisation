#include <thread>

#include <future>
#include "A_Star_PathfindingCPU.h"


const int threadsToUse = 6;//Amount of threads to run for farming
//const int neighbourModifier = 1;

#pragma region BASE_ASTAR
bool Base_A_Star_PathfindingCPU::SetUpStartAndEndNodes(Vector2<int> startPos, Vector2<int> endPos)
{
	//Run Async functions to find node from positions
	std::future<Node*> startNode_fut = std::async(std::launch::async, &Base_A_Star_PathfindingCPU::GetNodeFromPosition,this, startPos);
	std::future<Node*> endNode_fut = std::async(std::launch::async, &Base_A_Star_PathfindingCPU::GetNodeFromPosition,this, endPos);

	//Set up root node
	Node* startNode = startNode_fut.get();
	if (startNode == nullptr)
		return false;
	startNode->SetGCost(0); startNode->SetParent(nullptr);
	startNode->SetHCost(floorf((endPos.x - startPos.x) + (endPos.y - startPos.y))); startNode->SetFCost();
	root = startNode;
	
	//Set up target node
	target = endNode_fut.get();
	if (target == nullptr)
		return false;

	//Set up root room
	for (Room* var : rooms)
	{
		if (IsNodeInRoom(*var, startNode->position))
		{
			startRoom = var;
			return true;
		}
	}


	return false;
}
/// <summary>
/// Searches all rooms to find the corresponding node with the same position as given
/// </summary>
Node* Base_A_Star_PathfindingCPU::GetNodeFromPosition(Vector2<int> position)
{
	//Get node from rooms based on position
	for (auto rm : rooms)
	{
		if (IsNodeInRoom(*rm, position))
		{
			return FindNodeInRoom(position, rm);
		}
	}
}
/// <summary>
/// Finds and returns the node in the room given, assuming the coordinates match. Will shift as necessary for any position not comforming to the node size
/// </summary>
Node* Base_A_Star_PathfindingCPU::FindNodeInRoom(Vector2<int> pos, Room* rm)
{
	//Trying to get the reference of the room position back to 0 to equalise everything
	Vector2<int> balance = rm->GetLowestCoord();
	Vector2<int> tempPos = pos;
	tempPos -= balance;

	//We are now working in an assumed 1x1 grid. Get array from grid position
	int x, y;
	x = 0;
	y = 0;
	x = tempPos.x; y = tempPos.y;
	
	return &rm->nodes[x][y];

}
///////Acknowledgment: The IsNodeInRoom functions are based on the CMP105 project - collision detection
//Collision detection code for finding if position is within certain bounds
bool Base_A_Star_PathfindingCPU::IsNodeInRoom(const RoomStruct& nm, const Vector2<int> position)
{
	if (nm.GetHighestCoord().x < position.x) { return false; }
	if (nm.GetLowestCoord().x > position.x) { return false; }
	if (nm.GetHighestCoord().y < position.y) { return false; }
	if (nm.GetLowestCoord().y > position.y) { return false; }

	return true;
}


void Base_A_Star_PathfindingCPU::EditNode(Vector2<int> nodePos, NodeType changeTo)
{
	//Get the node
	Node* changedNode = GetNodeFromPosition(nodePos);
	if (changedNode == nullptr)
		return;

	if (changedNode->nodeType == Routes)
		return;
	//Change the node
	changedNode->nodeType = changeTo;
}
void Base_A_Star_PathfindingCPU::PrintRoute()
{
	//Changes most recursive nodes parents to 'Path' from target node
	Node* cn; cn = target;
	if (cn == nullptr)
		return;
	while (cn->GetParent() != nullptr)
	{
		if (cn->nodeType != Routes)
		{
			cn->nodeType = Path;
		}
		cn = cn->GetParent();
	}

	//Changes root node
	if (cn->nodeType != Routes || cn->nodeType != Obstacle)
	{
		cn->nodeType = Free;
	}

}
void Base_A_Star_PathfindingCPU::ResetRoute()
{
	//Changes most recursive nodes parents to 'Free' from target node and sets parent to nullptr afterwards
	Node* cn; cn = target;
	if (cn == nullptr)
		return;
	while (cn->GetParent() != nullptr)
	{
		if (cn->nodeType != Routes)
		{
			cn->nodeType = Free;
		}
		Node* oldCN = cn;
		cn = cn->GetParent();
		oldCN->SetParent(nullptr);
	}

	//Resets root node
	if (cn->nodeType != Routes || cn->nodeType != Obstacle)
	{
		cn->nodeType = Free;
	}

	//Reset Rooms
	for (auto rm : rooms)
	{
		if (rm->GetParentRoom() != nullptr)
		{
			rm->SetParentRoom(nullptr);
		}
	}
}
#pragma endregion


#pragma region SEGMENTED

/// <summary>
/// Specific to segmented variant
/// </summary>
bool A_Star_Pathfinding_Defined_SegmentedCPU::AStarAlgorithm()
{
	std::queue<Node*> tempTargets;//temporary target nodes that join rooms together towards main target node - these derive from the 'route' nodes that connect rooms

	//Search for a room path and push the 'Route' nodes into tempTargets
	if (!IsNodeInRoom(*startRoom, target->position)) {
		tempTargets = FindRouteNodePaths(BruteForcePathfindMaps());//temporary target nodes that join rooms together towards main target node
	}
	else {
		tempTargets.push(root);//Required for same room pathfinding
	}
	tempTargets.push(target);

	return MultiThreadedPathfinding(&tempTargets);
}
std::queue<Node*> A_Star_Pathfinding_Defined_SegmentedCPU::FindRouteNodePaths(std::stack<RoomStruct*> mapRoute)
{
	//Using the map route, find the corrseponding 'route' nodes that would connect the rooms together

	std::queue<Node*> temporaryTargets;//'Route'.teleportTo nodes to travel to, on the way towards main end node
	temporaryTargets.push(root);

	while (!mapRoute.empty())
	{
		Node* rn = FindRouteNode(mapRoute);
		if (rn == nullptr)
		{
			//We have not found a path or mapRoute is now empty
			break;
		}
		temporaryTargets.push(rn);
	}
	return temporaryTargets;
}
std::stack<RoomStruct*> A_Star_Pathfinding_Defined_SegmentedCPU::BruteForcePathfindMaps()
{
	//TODO: All rooms are linked but some paths are unavailable. Maybe do a preliminary check of some sort?
	//We will start searching from our start room until we have a path from start to end.

	std::stack<RoomStruct*> mapRoute;//Rooms we need to go through
	RoomStruct* currentRoomToSearch = startRoom;//Our current room that we're in

	std::set<RoomStruct*> open; open.insert(currentRoomToSearch);
	std::set<RoomStruct*> closed;
	//Brute force searching of all rooms
	while (open.size() != 0)
	{
		currentRoomToSearch = *open.begin();
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

			//If room is our target, start pushing path into mapRoute//otherwise add it to the open set
			if (IsNodeInRoom(*neighbour, target->position))
			{
				//Push room path into mapRoute and return
				RoomStruct* route = neighbour;
				while (route->GetParentRoom() != nullptr)
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

	return mapRoute;//Returns an empty std::stack
}
Node* A_Star_Pathfinding_Defined_SegmentedCPU::FindRouteNode(std::stack<RoomStruct*>& mapRoute)
{
	RoomStruct* map = mapRoute.top();
	mapRoute.pop();
	if (mapRoute.size() == 0)
	{
		return nullptr;
	}

	for (auto routeNode : map->GetRouteNodes())
	{
		Node* nodeInNextRoom = routeNode->nodeToTeleportTo;
		if (nodeInNextRoom != nullptr)
		{
			if (IsNodeInRoom(*mapRoute.top(), nodeInNextRoom->position))//If the routeNode's teleporting node is in the next room, return the routeNode
			{
				return routeNode;
			}
		}

	}
	return nullptr;
}

bool A_Star_Pathfinding_Defined_SegmentedCPU::MultiThreadedPathfinding(std::queue<Node*>* targetNodes)
{
	bool successful = true;
	std::vector<std::thread> threads;
	std::mutex lock;

	for (int numberOfThreads = 0; numberOfThreads < threadsToUse; numberOfThreads++)
	{
		threads.push_back(std::thread([&]() {
			//Pathfind to each target node until we run out - (by that point we should be in the same room as the target node)
			while (!targetNodes->empty())
			{
				lock.lock();
				if (targetNodes->empty())
				{
					lock.unlock();
					return;
				}

				//First get root, then the nodeToTeleportTo from all other route nodes
				Node* tempStartNode = targetNodes->front();
				if (tempStartNode->nodeType == Routes)
				{
					TeleportNode* tpNode = static_cast<TeleportNode*>(tempStartNode);
					tpNode->nodeToTeleportTo->SetParent(tpNode);//TODO: I feel like I can get rid of this by simply making sure all teleported nodes set their teleporting node as parents
					tempStartNode = tpNode->nodeToTeleportTo;
				}
				//TODO: think of a way to get rid of this lock check?
				targetNodes->pop();
				if (targetNodes->empty())//Latest thread has reached the target node
				{
					lock.unlock();
					return;
				}
				Node* tempTargetNode = targetNodes->front();
				lock.unlock();

				if (!DefaultAStar(tempStartNode, tempTargetNode))
				{
					successful = false;
				}
			}
			}));
	}

	std::for_each(threads.begin(), threads.end(), [](std::thread& t)
		{
			t.join();
		});

	//If one pathfinding path failed, then we return false
	if (successful)
	{
		return true;
	}
	return false;
}



/// <summary>
/// Designed to be as modular in use as possible
/// </summary>
/// <param name="startNode"></param>
/// <param name="endNode"></param>
/// <returns></returns>
bool A_Star_Pathfinding_Defined_SegmentedCPU::DefaultAStar(Node* startNode, Node* endNode)
{
	
	std::set<Node*, ReverseComparator> open;
	std::set<Node*> closed;
	open.insert(startNode);

	int iterations = 0;
	while (open.size() != 0 && iterations <= 1000000)
	{

		//Find lowest fCost Open Node
		Node* current = *open.begin();

		//If we found end, stop pathfinding
		if (Node::GetDistance(*current, *endNode) < 1)
		{
			if (man)
			{
				//std::vector<Node*> check;
				//Node* ci; ci = current;
				//while (ci->parentNode != nullptr)
				//{
				//	check.push_back(ci);
				//	ci = ci->parentNode;
				//}
				return true;
			}
			return true;
		}
		std::set<Node*>::iterator it = open.begin();//For some reason, it won't erase some points, so we need to point to the first one to erase instead of erasing the specific node

		//Restructure the node collections
		open.erase(it);
		closed.insert(current);

		//Neighbours
		CheckNeighbours(current, endNode, &open, &closed);

		iterations++;
	}
	return false;
}

void A_Star_Pathfinding_Defined_SegmentedCPU::CheckNeighbours(Node* current, Node* targetNode, std::set<Node*, ReverseComparator>* open, std::set<Node*>* closed)
{
	//SUMMARY: Neighbours does not gain any advantage by being multithreaded.
	//std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
#pragma region ORIGINAL
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

		int newGCost = current->GetGCost() + Node::GetDistance(*neighbour, *current);
		int newHCost = Node::GetDistance(*neighbour, *targetNode);
		int newFCost = newGCost + newHCost;


		bool inToSearch = open->find(neighbour) != open->end();
		//If neighbour is in either open, and the new path is better, recalculate
		if (inToSearch && neighbour->GetGCost() > newGCost)
		{
			open->erase(neighbour);
			neighbour->SetGCost(newGCost);
			neighbour->SetHCost(newHCost);
			neighbour->SetFCost(newFCost);
			neighbour->SetParent(current);
			open->insert(neighbour);
		}
		//If neighbour is not in open, set
		if (!inToSearch)
		{
			neighbour->SetGCost(newGCost);
			neighbour->SetHCost(newHCost);
			neighbour->SetFCost(newFCost);
			neighbour->SetParent(current);
			open->insert(neighbour);
		}

	}
#pragma endregion
#pragma region LIMITED
	////Last time we benchmarked, this was a heavy usage.
	//int counter = threadsToUse * neighbourModifier;

	//auto categoriseNeighbour = [&](auto neighbour) {
	//	//std::cout << "Detatched thread with counter " << counter << std::endl;
	//	//Parent node
	//	if (neighbour == nullptr || neighbour->nodeType == Obstacle || closed->find(neighbour) != closed->end())
	//	{
	//		counter++;
	//		return;
	//	}
	//	
	//	if (neighbour->position == targetNode->position)
	//	{
	//		targetNode->SetParent(current);
	//		targetNode->SetFCost(0);
	//	}

	//	int newGCost = current->GetGCost() + 1;
	//	bool inToSearch = open->find(neighbour) == open->end();
	//	//If neighbour is in either open, and the new path is better, recalculate
	//	if (!inToSearch && neighbour->GetGCost() > newGCost)
	//	{
	//		neighbour->SetGCost(newGCost);
	//		neighbour->SetFCost();
	//		neighbour->SetParent(current);
	//		counter++;
	//		return;
	//	}
	//	//If neighbour is not in open, set
	//	if (inToSearch)
	//	{
	//		int newHCost = Node::DistanceBetweenE(*neighbour, *targetNode);
	//		neighbour->SetGCost(newGCost);
	//		neighbour->SetHCost(newHCost);
	//		neighbour->SetFCost();
	//		neighbour->SetParent(current);
	//		open->insert(neighbour);

	//	}
	//	counter++;
	//	return;
	//};

	////For all neighbours : categorise them
	//for (auto neighbour : current->neighbours)
	//{
	//	//std::cout << "Main thread with counter " << counter << std::endl;
	//	counter--;
	//	std::thread(categoriseNeighbour, neighbour).detach();
	//	while (counter == 0)
	//	{
	//		//std::cout << "Main Neighbour thread waiting \n";
	//	}

	//}
	//while (counter != (threadsToUse * neighbourModifier))
	//{

	//}
#pragma endregion
#pragma region FARM
//		auto categoriseNeighbour = [&](auto neighbourNumber) {
//			//std::cout << "Thread neighbour number " << neighbourNumber << std::endl;
//
//			Node* neighbour = current->neighbours[neighbourNumber];
//		//std::cout << "Detatched thread with counter " << counter << std::endl;
//		//Parent node
//		if (neighbour == nullptr || neighbour->nodeType == Obstacle || closed->find(neighbour) != closed->end())
//		{
//			return;
//		}
//		
//		if (neighbour->position == targetNode->position)
//		{
//			targetNode->SetParent(current);
//			targetNode->SetFCost(0);
//		}
//
//		int newGCost = current->GetGCost() + 1;
//		bool inToSearch = open->find(neighbour) == open->end();
//		//If neighbour is in either open, and the new path is better, recalculate
//		if (!inToSearch && neighbour->GetGCost() > newGCost)
//		{
//			neighbour->SetGCost(newGCost);
//			neighbour->SetFCost();
//			neighbour->SetParent(current);
//			return;
//		}
//		//If neighbour is not in open, set
//		if (inToSearch)
//		{
//			int newHCost = Node::DistanceBetweenE(*neighbour, *targetNode);
//			neighbour->SetGCost(newGCost);
//			neighbour->SetHCost(newHCost);
//			neighbour->SetFCost();
//			neighbour->SetParent(current);
//			open->insert(neighbour);
//
//		}
//		return;
//	};
//std::vector<std::thread> threads;
//std::mutex lock;
//int neighbours = 7;
//for (int numberOfThreads = 0; numberOfThreads < threadsToUse*neighbourModifier; numberOfThreads++)
//{
//	threads.push_back(std::thread([&]() {
//		//Pathfind to each target node until we run out(by that point we should be in the same room
//		while (neighbours >= 0)
//		{
//			lock.lock();
//			if (neighbours < 0)
//			{
//				lock.unlock();
//				return;
//			}
//			int i = neighbours;
//			neighbours--;
//			lock.unlock();
//			categoriseNeighbour(i);
//
//		}
//		}));
//}
//
//std::for_each(threads.begin(), threads.end(), [&](std::thread& t)
//	{
//		t.join();
//	});
//
#pragma endregion

//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//auto ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
//	std::cout << ms << std::endl;
	//std::cout << "Main thread exiting";
}

#pragma endregion

