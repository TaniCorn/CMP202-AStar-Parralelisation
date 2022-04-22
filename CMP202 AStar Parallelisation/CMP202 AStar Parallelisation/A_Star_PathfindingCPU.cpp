#include <thread>

#include <future>
#include "A_Star_PathfindingCPU.h"
#pragma region BASE_ASTAR

const int threadsToUse = 1;
const int neighbourModifier = 1;


bool Base_A_Star_PathfindingCPU::SetUpStartAndEndNodes(Vector2<int> startPos, Vector2<int> endPos)
{
	//Run Async functions
	std::future<Node*> startNode_fut = std::async(std::launch::async, &Base_A_Star_PathfindingCPU::GetNodeFromPosition,this, startPos);
	std::future<Node*> endNode_fut = std::async(std::launch::async, &Base_A_Star_PathfindingCPU::GetNodeFromPosition,this, endPos);

	//Set up startnode
	Node* startNode = startNode_fut.get();
	if (startNode == nullptr)
		return false;
	startNode->SetGCost(0); startNode->SetParent(nullptr);
	startNode->SetHCost(floorf((endPos.x - startPos.x) + (endPos.y - startPos.y))); startNode->SetFCost();
	root = startNode;
	
	target = endNode_fut.get();
	if (target == nullptr)
		return false;

	return true;
}
Node* Base_A_Star_PathfindingCPU::GetNodeFromPosition(Vector2<int> position)
{
	//Get nodes from rooms based on position
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
/// <param name="pos"></param>
/// <param name="rm"></param>
/// <returns></returns>
Node* Base_A_Star_PathfindingCPU::FindNodeInRoom(Vector2<int> pos, Room* rm)
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

}


void Base_A_Star_PathfindingCPU::PrintRoute()
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
bool Base_A_Star_PathfindingCPU::IsNodeInRoom(const RoomStruct& nm, const Node& target)
{
	if (nm.GetHighestCoord().x < target.position.x) { return false; }
	if (nm.GetLowestCoord().x > target.position.x) { return false; }
	if (nm.GetHighestCoord().y < target.position.y) { return false; }
	if (nm.GetLowestCoord().y > target.position.y) { return false; }

	return true;
}

//Collision detection code for finding if position is within certain bounds
bool Base_A_Star_PathfindingCPU::IsNodeInRoom(const RoomStruct& nm, const Vector2<int> position)
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
bool A_Star_Pathfinding_Defined_SegmentedCPU::AStarAlgorithm()
{
	Node* current = root;
	Room* roomCheck = currentRoom;
	std::queue<Node*> tempTargets;// = FindRouteNodePaths(BruteForcePathfindMaps());//temporary target nodes that join rooms together towards main target node

	//While we're not in the same room, search for a path towards the room where our target node is in
	if (!IsNodeInRoom(*currentRoom, *target)) {
		tempTargets = FindRouteNodePaths(BruteForcePathfindMaps());//temporary target nodes that join rooms together towards main target node
	}
	tempTargets.push(target);

	return MultiThreadedPathfinding(&tempTargets);
}

bool A_Star_Pathfinding_Defined_SegmentedCPU::MultiThreadedPathfinding(std::queue<Node*>* targetNodes)
{
	bool successful = true;
	std::vector<std::thread> threads;
	std::mutex lock;

	for (int numberOfThreads = 0; numberOfThreads < threadsToUse; numberOfThreads++)
	{
		threads.push_back(std::thread([&]() {
			//Pathfind to each target node until we run out(by that point we should be in the same room
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
				targetNodes->pop();
				if (targetNodes->empty())
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


	if (successful)
	{
		return true;
	}
	return false;
}

std::stack<RoomStruct*> A_Star_Pathfinding_Defined_SegmentedCPU::BruteForcePathfindMaps()
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

std::queue<Node*> A_Star_Pathfinding_Defined_SegmentedCPU::FindRouteNodePaths(std::stack<RoomStruct*> mapRoute)
{

	std::queue<Node*> temporaryTargets;//target nodes to travel to, on the way towards main end node
	temporaryTargets.push(root);

	while (!mapRoute.empty())
	{
		Node* rn = FindRouteNode(mapRoute);
		if (rn == nullptr)
		{
			//TODO: We have not foudn a path or map is now empty
			break;
		}
		temporaryTargets.push(rn);
	}
	return temporaryTargets;
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
bool A_Star_Pathfinding_Defined_SegmentedCPU::DefaultAStar(Node* startNode, Node* endNode)
{
	std::set<Node*, ReverseComparator> open;
	std::set<Node*> closed;
	open.insert(startNode);
	std::thread threads[3];

	iterations = 0;
	while (open.size() != 0 && iterations <= 1000000)
	{

		//Find lowest fCost Open Node
		Node* current = *open.begin();
		//std::cout << current->position;
		//If we found end, stop pathfinding
		//int o = current->DistanceFromM(endNode.position);//debugging
		if (current->DistanceFromM(endNode->position) < 1)
		{
			return true;
		}
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
			int newHCost = Node::DistanceBetweenM(*neighbour, *targetNode);
			neighbour->SetGCost(newGCost);
			neighbour->SetHCost(newHCost);
			neighbour->SetFCost();
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
	//		int newHCost = Node::DistanceBetweenM(*neighbour, *targetNode);
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
//#pragma region FARM
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
//			int newHCost = Node::DistanceBetweenM(*neighbour, *targetNode);
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
//#pragma endregion

//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//auto ms = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
//	std::cout << ms << std::endl;
	//std::cout << "Main thread exiting";
}

#pragma endregion

