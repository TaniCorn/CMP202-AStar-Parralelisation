//////////Room Files
//////////Contains Struct RoomStruct which holds all data for a Room of Nodes. Contains Class Room : RoomStruct which provides functionality for the Rooms,
////////// this functionality is linked to the Map File
//////////Written by Tanapat Somrid 
/////////Starting 18/12/2021
//////// Most Recent Update 07/05/2022
//////// Most Recent change: Cleanup, Rename of file to Rooms.h

//TODO: Make Roomstruct a subset of Node. So the pathfinding algorithm can use A* on rooms as well : A great speed boost for around 4*3 Bytes of information :)
#pragma once
#ifndef ROOMS_H
#define ROOMS_H

#include "Node.h"
#include <set>
#include "CellularAutomata.h"

/// <summary>
/// Contains all the data for a Room. Can also be acted upon by a pathfinding algorithm as it has neighbouring rooms and a parent room for paths.
/// </summary>
struct RoomStruct {
public:
	RoomStruct() {
		parent = nullptr;
	}
	~RoomStruct() {
		for (int i = 0; i < xSize - 1; ++i)
		{
			delete[] nodes[i];
		}

	}

	Node** nodes;//The nodes in the room(TODO:Explore graphs one day?)

protected:
	//Dimensions of Room
	//Friendly reminder to self. Don't do room[xSize][ySize] because that is out of bounds. Make sure to minus it by 1.
	int xSize;//Size of Room in x-Axis
	int ySize;//Size of Room in y-Axis
	int lowestXCoord, lowestYCoord, highestXCoord, highestYCoord;//Room boundary coordinates.

	//Pathfinding Data - Data to be used when pathfinding through a set of Rooms
	std::vector<TeleportNode*> routeNodes;//Nodes that connect to other rooms
	std::set<RoomStruct*> neighbouringRooms;//Neighbouring rooms, can have multiple(Possibility for traversal of levels up and down through stairs)
	RoomStruct* parent;//Parent room of pathfinding rooms
	//TODO: Add hCost, fCost, gCost for A*


public:
	//Dimensions of Room Getters/Setters
	int GetXSize() const { return xSize; } void SetXSize(int x) { xSize = x; }
	int GetYSize()const { return ySize; } void SetYSize(int y) { ySize = y; }
	int GetLowestXCoord()const { return lowestXCoord; } int GetHighestXCoord() const { return highestXCoord; }
	int GetLowestYCoord()const { return lowestYCoord; } int GetHighestYCoord() const { return highestYCoord; }
	void SetLowestCoord(int x, int y) { lowestXCoord = x; lowestYCoord = y; } 
	void SetHighestCoord(int x, int y) { highestXCoord = x; highestYCoord = y; }

	//Pathfinding Data Getters/Setterse
	std::set<RoomStruct*> GetNeighbouringRooms() const { return neighbouringRooms; } void SetNeighbouringRooms(std::set<RoomStruct*> nr) { neighbouringRooms = nr; } void AddNeighbouringRoom(RoomStruct* room) { neighbouringRooms.insert(room); }
	RoomStruct* GetParentRoom() const { return parent; } void SetParentRoom(RoomStruct* rs) { parent = rs; }
	std::vector<TeleportNode*> GetRouteNodes() { return routeNodes; } void SetRouteNodes(std::vector<TeleportNode*> n) { routeNodes = n; } void AddRouteNode(TeleportNode* node) { routeNodes.push_back(node); }

};

/// <summary>
/// Gives functionality for linking nodes
/// </summary>
class Room : public RoomStruct {
public:
	//TODO: Perhaps this function should go in the struct as it is quite essential for a simple room to work
	/// <summary>
	/// Links all the nodes with their neighbouring nodes, the room parameter was in place to allow me to use it as a static function.
	/// </summary>
	/// <param name="nm"> self call</param>
	void LinkNeighbours(Room& nm);

	/// <summary>
	/// Sets eachother's .nodeToTeleportTo to eachother. A bi-directional pathway
	/// </summary>
	void DualLinkRouteNodes(TeleportNode* node1, TeleportNode* node2);
	/// <summary>
	/// Sets node1.nodeToTeleportTo to the teleportingToNode. A uni-directional pathway
	/// </summary>
	void SingleLinkRouteNodes(TeleportNode* node1, Node* teleportingToNode);

	/// <summary>
	/// Generates the Room using cellular Automata
	/// </summary>
	/// <param name="xDimension"></param>
	/// <param name="yDimension"></param>
	/// <param name="xOrigin"> The top left corner coordinate</param>
	/// <param name="yOrigin"> The top left corner coordinate</param>
	void GenerateAutomataRoom(int xDimension, int yDimension, int xOrigin, int yOrigin);

	TeleportNode* SpawnRouteNode(int locationX, int locationY);
};

#endif // !ROOMS_H
