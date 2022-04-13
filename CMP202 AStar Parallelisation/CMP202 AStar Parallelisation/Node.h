//////////
//////////Node Map files
//////////Written by Tanapat Somrid 
/////////Starting 08/12/2021
//////// Most Recent Update 12/04/2022
//////// Most Recent change: Got rid of bloat


#pragma once
#ifndef NODE_H
#define NODE_H

#include "VectorPositions.h"
#include <vector>

static int inclination = 2;//How weighted should our path be towards the target node?

enum NodeType
{
	Free = 0,
	Obstacle = 1,
	Routes = 2,
};

/// <summary>
/// A bit late to use now, it would take a lot of re-writing tiny bits of code that don't ultimatley matter
/// </summary>
enum Direction {
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,
	NORTHWEST = 4,
	NORTHEAST = 5,
	SOUTHEAST = 6,
	SOUTHWEST = 7,
};



/// <summary>
/// This node class is a 
/// The node class uses a tree system.
/// </summary>
class Node {
public:
#pragma region CONSTRUCTORS
	/// <summary>
	/// Constructor For Start and End Nodes
	/// </summary>
	Node() {
		nodeType = Free; parentNode = nullptr;
	}
	/// <summary>
	/// Constructor for new Nodes
	/// </summary>
	Node(const Vector2<int>& newPosition) {
		nodeType = Free;
		position = newPosition;
		parentNode = nullptr;
	}
	Node(int x, int y) {
		nodeType = Free;
		position.x = x; position.y = y;
		parentNode = nullptr;
	}

	~Node() {

	}
	/////////Copy Constructors

	/// <summary>
	/// Copy Constructor for assignment
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	Node(const Node& a) { fCost = a.fCost; gCost = a.gCost; hCost = a.hCost; position = a.position; nodeType = a.nodeType; parentNode = a.parentNode; for (int i = 0; i < 4; i++) { neighbours[i] = a.neighbours[i]; }; }
	Node& operator =(const Node& copy) { fCost = copy.fCost; gCost = copy.gCost; hCost = copy.hCost; position = copy.position; nodeType = copy.nodeType; parentNode = copy.parentNode; for (int i = 0; i < 4; i++) { neighbours[i] = copy.neighbours[i]; }; return *this; }

	void SetNullNeighbours() {
		for (int i = 0; i < 8; i++)
		{
			neighbours[i] = nullptr;
		}
	}
#pragma endregion

public:
#pragma region DISTANCES
	/// <summary>
/// The EUCLIDEAN distance from this node and the given coordinates (x and y)
/// </summary>
/// <returns>EUCLIDEAN DISTANCE</returns>
	int DistanceFromE(int x, int y) { return position.DistanceFrom(x, y); }
	/// <summary>
	/// The EUCLIDEAN distance from this node and the given vector (position.x and position.y)
	/// </summary>
	/// <returns>EUCLIDEAN DISTANCE</returns>
	int DistanceFromE(const Vector2<int>& pos) {
		return position.DistanceFrom(pos);
	}
	/// <summary>
	/// The EUCLIDEAN distance between two given nodes
	/// <para> From Node A to Node B</para>
	/// </summary>
	/// <param name="Node A"></param>
	/// <param name="Node B"></param>
	/// <returns>EUCLIDEAN DISTANCE</returns>
	static int DistanceBetweenE(const Node& a, const Node& b) {
		return Vector2<int>::DistanceBetween(a.position, b.position);
	}
	/// <summary>
/// The MANHATTAN distance from this node and the given coordinates (x and y)
/// </summary>
/// <returns>MANHATTAN DISTANCE</returns>
	int DistanceFromM(int x, int y) { return (abs(x - position.x) + abs(y - position.y)); }
	/// <summary>
	/// The MANHATTAN distance from this node and the given vector (position.x and position.y)
	/// </summary>
	/// <returns>MANHATTAN DISTANCE</returns>
	int DistanceFromM(const Vector2<int>& pos) {
		int total = (abs(pos.x - position.x) + abs(pos.y - position.y));
		return total;
	}
	/// <summary>
	/// The MANHATTAN distance between two given nodes
	/// <para> From Node A to Node B</para>
	/// </summary>
	/// <param name="Node A"></param>
	/// <param name="Node B"></param>
	/// <returns>MANHATTAN DISTANCE</returns>
	static int DistanceBetweenM(const Node& a, const Node& b) {
		int total = (abs(a.position.x - b.position.x) + abs(a.position.y - b.position.y));
		return total;
	}
#pragma endregion


#pragma region CHARACTERISTICS
private:
	float fCost = -1;//totalCost
	float gCost = -1;//cost of current path
	float hCost = -1;//how far away we are from target Node
public:
	Vector2<int> position = Vector2<int>(0, 0);//Our position, it's unique


	Node* neighbours[8];//Surrounding nodes
	Node* parentNode = nullptr;//Node that this node branched from
	//bool closed = false;//Is this closed? We could either check if this position exists in closed or just do this.

	NodeType nodeType;//Is this an obstacle? If so we can't pass through it
#pragma endregion

public:
#pragma region GETTERS/SETTERS
	void SetHCost(int h) {
		hCost = h * inclination;
	}
    int GetHCost() const {
		return hCost;
	}
	void SetGCost(int g) {
		gCost = g;
	}
	int GetGCost() const {
		return gCost;
	}	
	//Calculates and assigns with inclination
	void SetFCost() {
		fCost = hCost + gCost;
	}
	//Not preferred
	void SetFCost(int f) {
		fCost = f;
	}
	int GetFCost() const {
		return fCost;
	}
	void SetParent(Node* parent) {
		parentNode = parent;
	}
	Node* GetParent() const {
		return parentNode;
	}
#pragma endregion

};

#endif // !NODEMAP_H
