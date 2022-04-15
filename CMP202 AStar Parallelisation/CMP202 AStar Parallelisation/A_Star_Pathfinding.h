//////////
//////////A Star Pathfing files // PathfindingMap.h required
//////////Written by Tanapat Somrid 
/////////Starting 01/12/2021
//////// Most Recent Update 15/04/2022
//////// Most Recent change: After running through the program, i've now ensured that the program works as intended. Many minor bugfixes have been made, see edits in git to see more. 
// ///// TODO: When setting the path to 'Path' it also sets all 'Route' nodes, fix
//////// TODO: Get a lot of error checking and catch cases where the path cannot be found. Currently the algorithms mostly assume it's possible. Make undefined have more directions
//////// TODO: Ideally, for actual use, the algorithm won't update continually only when(player has moved to a different room, player has moved position in same room);

//This has been done while thinking of using it for a 2d game utilising square rooms where enemies can spawn. 
//This was inspired by my own project last year for CMP105, and as such some of the terminology/comments may refer to that instead of a general term, afterall this has been designed around that use case.


#pragma once

#ifndef A_STAR_PATHFINDING_H
#define A_STAR_PATHFINDING_H

#include <stack>
#include <queue>
#include "PathfindingMap.h"

//For some reason the priority queue and node have different ways of comparing so I have to use two different comparator

#pragma region COMPARATOR

/// <summary>
/// Checks the smallest fcost, smallest h cost
/// </summary>
struct ReverseComparator {
public:
	bool operator ()(const Node* a, const Node* b) const {
		if (a->GetFCost() == b->GetFCost())
		{
			return a->GetHCost() > b->GetHCost();
		}
		return (a->GetFCost() < b->GetFCost());
	}
};
/// <summary>
/// Checks the smallest fcost, smallest h cost
/// </summary>
struct PriorityComparator {
public:
	bool operator ()(const Node* a, const Node* b) const {
		if (a->GetFCost() == b->GetFCost())
		{
			return a->GetHCost() < b->GetHCost();
		}
		return (a->GetFCost() > b->GetFCost());
	}
};

#pragma endregion

/// <summary>
/// Base A Star Class, provides base functionality, virtual functions and helper functions. Also provides limited member variables
/// </summary>
class Base_A_Star_Pathfinding {
public:
	Node* root;//Start node
	Node* target;//End node
	Room* currentRoom;//Contains Obstacle locations
	int nodeSize;//Allows us to take into account of the size of the agent using this(Assuming position is the center of agent, agent will not go halfway inside a wall.)

	int iterations;//For Debug and checking

	std::vector<Room*> rooms;//Rooms we're allowed to traverse


	#pragma region MANDATORY_BASE_FUNCTIONS
	void SetCurrentRoom(Room* nm) {
		currentRoom = nm;
		nodeSize = nm->GetNodeSize();
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

		FindCurrentRoom(startPos);
		SetUpStartAndEndNodes(startPos, endPos);
		AStarAlgorithm();
	};

protected:
	/// <summary>
	/// Is Node in current room//TODO: Fix function or fix name
	/// </summary>
	/// <param name="rootPosition"></param>
	virtual void FindCurrentRoom(const Vector2<int> rootPosition) { if (!Base_A_Star_Pathfinding::IsNodeInRoom(*currentRoom, rootPosition)) { return; } /*std::cout << "Room ok! \n";*/ };
	/// <summary>
	/// Translates the positions to the node format, and assigns each position their node.
	/// </summary>
	/// <param name="startPos"></param>
	/// <param name="endPos"></param>
	virtual void SetUpStartAndEndNodes(Vector2<int> startPos, Vector2<int> endPos);
	/// <summary>
	/// A star algorithm
	/// </summary>
	virtual void AStarAlgorithm() = 0;
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

//Todo: The segmented functions provide a much better base and maybe should be used instead?

/// <summary>
/// Optimises room functionality, when there are many obstacles to go through and the path may be more obscure than thought.
/// </summary>
class A_Star_Pathfinding_Defined_Segmented : public Base_A_Star_Pathfinding {
public:
	A_Star_Pathfinding_Defined_Segmented() {};
	~A_Star_Pathfinding_Defined_Segmented() {
	};

	std::set<Node*, ReverseComparator> toSearch;
	std::set<Node*> searched;


private:
	/// <summary>
	/// Searches all rooms to find our current room based on our position //Move to base?
	/// </summary>
	/// <param name="rootPosition"></param>
	void FindCurrentRoom(const Vector2<int> rootPosition) override;
	void AStarAlgorithm() override;

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

#endif // !A_STAR_PATHFINDING_H
