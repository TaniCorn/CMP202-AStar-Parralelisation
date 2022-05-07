//////////
//////////A Star Pathfinding CPU files //Derived from CMP201 Project A* Segmented.
//////////This version of A* is a parallelised version of the previous CMP201 project, with many fixes to the original code and improvements.
//////////Written by Tanapat Somrid 
/////////Starting 19/04/2022
//////// Most Recent Update 07/05/2022
//////// Most Recent change: Cleanup, removed dependency on old files, added FindPath(...) function to allow normal ints rather than Vector2<int>
//////// 
//Current Bug List:
//Will cause a GetParent() Error - When setting target node to a 'RouteNode'

#pragma once

#ifndef A_STAR_PATHFINDINGCPU_H
#define A_STAR_PATHFINDINGCPU_H

#include <stack>
#include <queue>
#include "Rooms.h"
#include "VectorPositions.h"

struct ReverseComparator {
public:
	bool operator ()(const Node* a, const Node* b) const {
		//If they have the same fCost, they could be the same return if they are or not. 
		//If they are the same, it won't insert it. If they are not, it will be inserted at the current location. 
		//This is needed to prevent non duplicates from not being inserted.
		if (a->GetFCost() == b->GetFCost())
		{
			return a < b;
		}
		//If they have the exact same position they are the same so therefore get rid of one
		if (a->xPosition == b->xPosition)
		{
			if (a->yPosition == b->yPosition)
			{
				return a < b;
			}
		}
		//Repositioning based on fCost
		return (a->GetFCost() < b->GetFCost());
	}
};

/// <summary>
/// Base A*. Provides functionality and data that is absolutely required. Does not contain the A* algorithm itself.
/// <para>Some functions are parallelised already</para>
/// </summary>
class Base_A_Star_PathfindingCPU {
public:
	int threadsToUse = std::thread::hardware_concurrency();
	Node* target;//End node
	Node* root;//Start node

	std::vector<Room*> rooms;//Rooms we're allowed to traverse
	#pragma region FUNCTIONS_TO_CALL
	/// <summary>
	/// Set the rooms that the agent can travel through and search.
	/// </summary>
	void SetTraversableRooms(std::vector<Room*> rooms) {
		this->rooms = rooms;
	}

	/// <summary>
	/// Resets any 'Path' nodes back to their default node
	/// </summary>
	void ResetRoute();
	/// <summary>
	/// Find a path using the position of self and target
	/// </summary>
	void FindPath(Vector2<int> startPos, Vector2<int> endPos) {
		ResetRoute();
		
		//Parallelised
		if (!SetUpStartAndEndNodes(startPos, endPos)) {
			std::cout << "STARTNODEENDNODE FAILED!" << std::endl;
			return;
		}
		if(!AStarAlgorithm())
			std::cout << "PATHFINDING FAILED!" << std::endl;
	};	
	void FindPath(int xStart,int yStart,int xEnd, int yEnd) {
		ResetRoute();
		Vector2<int> startPos = Vector2<int>(xStart, yStart);
		Vector2<int> endPos = Vector2<int>(xEnd, yEnd);

		//Parallelised
		if (!SetUpStartAndEndNodes(startPos, endPos)) {
			std::cout << "STARTNODEENDNODE FAILED!" << std::endl;
			return;
		}
		if (!AStarAlgorithm())
			std::cout << "PATHFINDING FAILED!" << std::endl;
	};
	/// <summary>
	/// Changes any 'Free' nodes - that are in a recursive 'target.parent' node - to 'Path' nodes
	/// </summary>
	void PrintRoute();
	#pragma endregion


	/// <summary>
	/// Change a node at the given location to be a different type
	/// </summary>
	void EditNode(Vector2<int> nodePos, NodeType typeToChangeTo);

	//DEBUGGING
	/// <summary>
	/// For debugging purposes. Prints gCost, fCost, and position of a node.
	/// </summary>
	//void PrintNode(Node* n) {
	//	std::cout << "GCost:" << n->GetGCost() << " | FCost:" << n->GetFCost() << " | Position:" << n->position << std::endl;
	//}


protected:
	Room* startRoom;//Room that the start node is in


	/// <summary>
	/// Finds the corresponding nodes of each position and assigns them to root and target. 
	/// <para>Also sets up startRoom.</para>
	/// </summary>
	virtual bool SetUpStartAndEndNodes(Vector2<int> startPos, Vector2<int> endPos);

	/// <summary>
	/// A star algorithm
	/// </summary>
	virtual bool AStarAlgorithm() = 0;

	/// <summary>
	/// Finds the corresponding node at the coordinates given. Searches all available rooms.
	/// </summary>
	Node* GetNodeFromPosition(Vector2<int> position);

	/// <summary>
	/// Checks if the position is within room bounds
	/// </summary>
	static bool IsNodeInRoom(const RoomStruct& nm, const Vector2<int> position);
private:
	/// <summary>
	///	If the nodes position exists in the room, return the node.
	/// <para>Helper function for SetUpStartAndEndNodes()</para>
	/// </summary>
	Node* FindNodeInRoom(Vector2<int> pos, Room* rm);


};

/// <summary>
/// Extends upon Base A* to provide intended functionality.
/// <para>Is parallelised by the CPU to speed up pathfinding process</para>
/// </summary>
class A_Star_Pathfinding_Defined_SegmentedCPU : public Base_A_Star_PathfindingCPU {
public:
	A_Star_Pathfinding_Defined_SegmentedCPU() {};
	~A_Star_Pathfinding_Defined_SegmentedCPU() {
	};


private:
	bool AStarAlgorithm() override;
	bool MultiThreadedPathfinding(std::queue<Node*>* targetNodes);
	/// <summary>
	/// Searches through the rooms to find a path towards the target in room format
	/// </summary>
	/// <returns></returns>
	std::stack<RoomStruct*> BruteForcePathfindMaps();
	/// <summary>
	/// Find the path for the route nodes connecting the rooms from the BruteForcePathfindMaps function
	/// </summary>
	/// <param name="mapRoute"></param>
	/// <returns></returns>
	std::queue<Node*> FindRouteNodePaths(std::stack<RoomStruct*> mapRoute);

	/// <summary>
	/// Default a star to pathfind basic routes.
	/// </summary>
	/// <param name="startNode"></param>
	/// <param name="endNode"></param>
	/// <returns></returns>
	bool DefaultAStar(Node* startNode, Node* endNode);

	void CheckNeighbours(Node* node, Node* targetNode, std::set<Node*, ReverseComparator>* open, std::set<Node*>* closed);
	

	/// <summary>
	/// Find the correct route nodes to use for pathfinding through the given rooms
	/// </summary>
	/// <param name="mapRoute"></param>
	/// <returns></returns>
	Node* FindRouteNode(std::stack<RoomStruct*>& mapRoute);
};

#endif // !A_STAR_PATHFINDINGCPU_H
