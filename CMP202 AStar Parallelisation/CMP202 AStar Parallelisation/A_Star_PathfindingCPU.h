//////////
//////////A Star Pathfinding CPU files // PathfindingMap.h required // Derived from A_Star_Pathfinding.h
//////////Written by Tanapat Somrid 
/////////Starting 19/04/2022
//////// Most Recent Update 20/04/2022
//////// Most Recent change: Cleanup Done
//////// 

#pragma once

#ifndef A_STAR_PATHFINDINGCPU_H
#define A_STAR_PATHFINDINGCPU_H

#include <stack>
#include <queue>
#include "PathfindingMap.h"
#include "A_Star_Pathfinding.h"
/// <summary>
/// Checks the smallest fcost, smallest h cost
/// </summary>
//struct ReverseComparator {
//public:
//	bool operator ()(const Node* a, const Node* b) const {
//		if (a->GetFCost() == b->GetFCost())
//		{
//			return a->GetHCost() > b->GetHCost();
//		}
//		return (a->GetFCost() < b->GetFCost());
//	}
//};


/// <summary>
/// Base A Star Class, provides base functionality, virtual functions and helper functions. Also provides limited member variables
/// </summary>
class Base_A_Star_PathfindingCPU {
public:
	Node* root;//Start node
	Node* target;//End node
	Room* currentRoom;//Contains Obstacle locations

	int iterations;//For Debug and checking

	std::vector<Room*> rooms;//Rooms we're allowed to traverse


	#pragma region MANDATORY_BASE_FUNCTIONS
	void SetCurrentRoom(Room* nm) {
		currentRoom = nm;
	}
	void SetTraversableRooms(std::vector<Room*> rooms) {
		this->rooms = rooms;
	}
	/// <summary>
	/// Find a path using the position of self and target
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="startPos"></param>
	/// <param name="endPos"></param>
	template <typename T> void FindPath(Vector2<T> startPos, Vector2<T> endPos) {
		startPos = Vector2<int>(startPos);
		endPos = Vector2<int>(endPos);

		if(!SetUpStartAndEndNodes(startPos, endPos));
		if(!AStarAlgorithm());
	};	

protected:
	/// <summary>
	/// Translates the positions to the node format, and assigns each position their node.
	/// </summary>
	/// <param name="startPos"></param>
	/// <param name="endPos"></param>
	virtual bool SetUpStartAndEndNodes(Vector2<int> startPos, Vector2<int> endPos);
	/// <summary>
	/// A star algorithm
	/// </summary>
	virtual bool AStarAlgorithm() = 0;

	Node* GetNodeFromPosition(Vector2<int> position);
private:
	/// <summary>
	/// Helper function for SetUpStartAndEndNodes()
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="rm"></param>
	/// <returns></returns>
	Node* FindNodeInRoom(Vector2<int> pos, Room* rm);

	#pragma endregion
public:
	#pragma region HELPER_FUNCTIONS
	static bool IsNodeInRoom(const RoomStruct& nm, const Node& n);
	static bool IsNodeInRoom(const RoomStruct& nm, const Vector2<int> position);
	#pragma endregion


	/// <summary>
	/// For Debuggin purposes
	/// </summary>
	void PrintPath() {

		Node* current = target;
		std::cout << " It " << iterations << "Times it took" << std::endl;

		while (current->GetParent() != nullptr)
		{
			current = current->GetParent();
			std::cout << current->position << std::endl;
			std::cout << current->GetGCost() << std::endl;
		}
	};
	void PrintNode(Node* n) {
		std::cout << "GCost:" << n->GetGCost() << " | FCost:" << n->GetFCost() << " | Position:" << n->position << std::endl;
	}
	void PrintRoute();
};

/// <summary>
/// Optimises room functionality, when there are many obstacles to go through and the path may be more obscure than thought.
/// </summary>
class A_Star_Pathfinding_Defined_SegmentedCPU : public Base_A_Star_PathfindingCPU {
public:
	A_Star_Pathfinding_Defined_SegmentedCPU() {};
	~A_Star_Pathfinding_Defined_SegmentedCPU() {
	};

	std::set<Node*, ReverseComparator> toSearch;
	std::set<Node*> searched;


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
