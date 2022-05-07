//////////
//////////Node
////////// 
//////////Written by Tanapat Somrid 
/////////Starting 08/12/2021
//////// Most Recent Update 07/05/2022
//////// Most Recent change: Cleanup, Removed usage of Vector2<int> will have knock on effect to other classes. Preferring Primitive data types when applicable


#pragma once
#ifndef NODE_H
#define NODE_H

#include <math.h>
//Theoritically if the path is relatively straightforward with only a few obstacles, this inclination should make it faster when used with hCost
static int inclination = 2;//How weighted should our path be towards the target node? 

enum NodeType
{
	Free = 0,
	Obstacle = 1,
	Routes = 2,

	Path = 3,//Will be used for the main path
	//Possibility to add a step by step walkthrough, we can add more variations later
	//Open = 4,
	//Closed = 5
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
	/// Defualt Constructor
	/// </summary>
	Node() {
	}

	~Node() {

	}

	//Copy and Assignment Constructors
	Node(const Node& a) { fCost = a.fCost; gCost = a.gCost; hCost = a.hCost; xPosition = a.xPosition; yPosition = a.yPosition; nodeType = a.nodeType; parentNode = a.parentNode; for (int i = 0; i < 8; i++) { neighbours[i] = a.neighbours[i]; }; }
	Node& operator =(const Node& copy) { fCost = copy.fCost; gCost = copy.gCost; hCost = copy.hCost; xPosition = copy.xPosition; yPosition = copy.yPosition; nodeType = copy.nodeType; parentNode = copy.parentNode; for (int i = 0; i < 8; i++) { neighbours[i] = copy.neighbours[i]; }; return *this; }
#pragma endregion

public:
#pragma region DISTANCES
	//After noticing my mistake of using floats with the previous int functions, i tried using floats but alas, float comparisons provide inaccurate results.
	//So I had to switch to an int *10 factor, this function provides a easy Euclidean distance calculation. Many thanks to my all time favourite Sebastian Lague
	static int GetDistance(const Node& a, const Node& b) {
		int distX = abs(a.xPosition - b.xPosition);
		int distY = abs(a.yPosition - b.yPosition);

		if (distX > distY)
			return (14 * distY + (10 * (distX - distY)));
		return (14 * distX + (10 * (distY - distX)));

	}
#pragma endregion


#pragma region CHARACTERISTICS
protected:
	int fCost = -1;//totalCost
	int gCost = -1;//cost of current path
	int hCost = -1;//how far away we are from target Node
public:
	//Vector2<int> position = Vector2<int>(0, 0);//Our position, it's unique
	int xPosition = 0;
	int yPosition = 0;


	Node* neighbours[8];//Surrounding nodes
	Node* parentNode = nullptr;//Node that this node branched from
	//bool closed = false;//Is this closed? We could either check if this position exists in closed or just do this.

	NodeType nodeType;//Is this an obstacle? If so we can't pass through it
#pragma endregion

public:
#pragma region GETTERS/SETTERS
	void SetHCost(int h) {
	hCost = h;// *inclination;
	}
    int GetHCost() const {return hCost;}

	void SetGCost(int g) {	gCost = g;}
	int GetGCost() const {return gCost;}	

	//Calculates and assigns with inclination
	void SetFCost() {	fCost = hCost + gCost;}
	//Not preferred
	void SetFCost(int f) {	fCost = f;}
	int GetFCost() const {	return fCost;}

	void SetParent(Node* parent) {	parentNode = parent;}
	Node* GetParent() const {	return parentNode;}
#pragma endregion

};

class TeleportNode : public Node {
public:
	Node* nodeToTeleportTo;
	TeleportNode() { nodeType = Routes; };

	//Copy and Assignment Constructors
	TeleportNode(const Node& a) { xPosition = a.xPosition; yPosition = a.yPosition; for (int i = 0; i < 8; i++) { neighbours[i] = a.neighbours[i]; } }
	TeleportNode(const TeleportNode& a) { xPosition = a.xPosition; yPosition = a.yPosition; for (int i = 0; i < 8; i++) { neighbours[i] = a.neighbours[i]; }}
	TeleportNode& operator =(const Node& copy) { xPosition = copy.xPosition; yPosition = copy.yPosition;  for (int i = 0; i < 8; i++) { neighbours[i] = copy.neighbours[i]; }return *this;}
	TeleportNode& operator =(const TeleportNode& copy) { xPosition = copy.xPosition; yPosition = copy.yPosition;  for (int i = 0; i < 8; i++) { neighbours[i] = copy.neighbours[i]; }return *this;}
};
#endif // !NODEMAP_H
