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
/// Base A Star Class, provides base functionality, virtual functions and helper functions. Also provides limited member variables
/// </summary>
class Base_A_Star_PathfindingCPU {
public:
	Node* target;//End node
	Node* root;//Start node
	bool man = false;
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
		startPos = Vector2<int>(startPos);
		endPos = Vector2<int>(endPos);
		
		if (!SetUpStartAndEndNodes(startPos, endPos)) {
			std::cout << "STARTNODEENDNODE FAILED!" << std::endl;
			return;
		}
		if(!AStarAlgorithm())
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
	void PrintNode(Node* n) {
		std::cout << "GCost:" << n->GetGCost() << " | FCost:" << n->GetFCost() << " | Position:" << n->position << std::endl;
	}


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
/// Optimises room functionality, when there are many obstacles to go through and the path may be more obscure than thought.
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
