
//////////
//////////Pathfinding Map files
//////////Written by Tanapat Somrid 
/////////Starting 18/12/2021
//////// Most Recent Update 18/01/2022
//////// Most Recent change: Cleaning Done, removed node size
//TODO: Changed the singly linked route node to also use teleport node
#pragma once
#ifndef PATHFINDINGMAP_H
#define PATHFINDINGMAP_H

#include "Node.h"
#include <set>
#include "CellularAutomata.h"

/// <summary>
/// Contains all the information of a node room. Can also be acted upon by a pathfinding algorithm as it has neighbouring rooms and a parent room for paths.
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
protected:
	int xSize;//Remember to access array elements that are -1 to this 
	int ySize;//Remember to access array elements -1 to this

public:
	Node** nodes;//The nodes in the room(TODO:Explore graphs)
protected:
	Vector2<int> lowestCoord, highestCoord;//Room boundary coordinates.
	std::vector<TeleportNode*> routeNodes;//Nodes that connect to other rooms
	std::set<RoomStruct*> neighbouringRooms;//Neighbouring rooms, can have multiple(Possibility for traversal of levels up and down through stairs)
	RoomStruct* parent;//Parent room of pathfinding rooms

public:
	int GetXSize() const { return xSize; } void SetXSize(int x) { xSize = x; }
	int GetYSize()const { return ySize; } void SetYSize(int y) { ySize = y; }
	Vector2<int> GetLowestCoord()const { return lowestCoord; } Vector2<int> GetHighestCoord() const { return highestCoord; }
	void SetLowestCoord(Vector2<int> low) { lowestCoord = low; } void SetHighestCoord(Vector2<int> high) { highestCoord = high; }
	std::set<RoomStruct*> GetNeighbouringRooms() const { return neighbouringRooms; } void SetNeighbouringRooms(std::set<RoomStruct*> nr) { neighbouringRooms = nr; } void AddNeighbouringRoom(RoomStruct* room) { neighbouringRooms.insert(room); }
	RoomStruct* GetParentRoom() const { return parent; } void SetParentRoom(RoomStruct* rs) { parent = rs; }
	std::vector<TeleportNode*> GetRouteNodes() { return routeNodes; } void SetRouteNodes(std::vector<TeleportNode*> n) { routeNodes = n; } void AddRouteNode(TeleportNode* node) { routeNodes.push_back(node); }

};

/// <summary>
/// Gives functionality for linking nodes
/// </summary>
class Room : public RoomStruct {
public:
	/// <summary>
	/// Links all the nodes with their neighbouring nodes, the room parameter was in place to allow me to use it as a static function.
	/// </summary>
	/// <param name="nm"> self call</param>
	void LinkNeighbours(Room& nm);
	/// <summary>
	/// Uni Directional: Link route nodes with calculation of direction
	/// </summary>
	/// <param name="node1">is the one linking</param>
	/// <param name="node2">is the nieghbour of n1, the one being linked</param>
	void LinkRouteNodes(Node& node1, Node& node2);

	void DualLinkRouteNodes(TeleportNode* node1, TeleportNode* node2);

	/// <summary>
	/// Generates a map using CellularAutomata.h - assigns base information to this.
	/// </summary>
	/// <param name="dimensions"></param>
	/// <param name="topLeftCornerPosition"></param>
	void GenerateAutomataRoom(Vector2<int> dimensions, Vector2<int> topLeftCornerPosition);

	TeleportNode* SpawnRouteNode(int locationX, int locationY);
};
class Map {
public:
	std::vector<Room*> roomsInMap;
};

#endif // !PATHFINDINGMAP_H
