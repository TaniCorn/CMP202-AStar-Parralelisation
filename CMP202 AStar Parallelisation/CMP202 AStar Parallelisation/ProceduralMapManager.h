//////////Procedural Map Manager
//////////This generates a grid of Rooms - A Map. Then Connects them via Route Nodes.
//////////Written by Tanapat Somrid 
/////////Starting 11/04/2022
//////// Most Recent Update 07/05/2022
//////// Most Recent change: Cleanup
#pragma once

#ifndef PROCEDURALMAPMANAGER_H
#define PROCEDURALMAPMANAGER_H
#include "Rooms.h"
#include <thread>
#include <mutex>

class ProceduralMapManager
{
public:
	ProceduralMapManager() { roomsInMap = nullptr; };
	~ProceduralMapManager() {
		for (int i = 0; i < xDimension; ++i)
		{
			delete [] roomsInMap[i];
		}
		delete[] roomsInMap;
	};
	bool failsafe = true;//Whens set to true, all rooms have guaranteed connection
	int threadsToUse = std::thread::hardware_concurrency();//Change for different amount of threads

	Room** roomsInMap;
	int xDimension, yDimension;
	int xRoomDimension, yRoomDimension;
	/// <summary>
	/// Sets Map and Room sizes
	/// </summary>
	/// <param name="xMapSize">How many Rooms along the right</param>
	/// <param name="yMapSize">How many Rooms vertically down</param>
	/// <param name="xRoomDimensions">How many Nodes in a Room on the x axis</param>
	/// <param name="yRoomDimensions">How many Nodes in a Room on the y axis</param>
	void Init(int xMapSize, int yMapSize, int xRoomDimensions, int yRoomDimensions) { xDimension = xMapSize; yDimension = yMapSize; xRoomDimension = xRoomDimensions; yRoomDimension = yRoomDimensions; }
	/// <summary>
	/// Generates all the Rooms for the Map
	/// </summary>
	void GenerateMapGrid();

	/// <summary>
	/// Adds a Route Node between all Rooms and connects them together
	/// <para>#Completely excluding the bottom right most Room</para>
	/// <para>#Partially excluding the bottom and right rooms - they are only singly connected</para>
	/// </summary>
	void ConnectRooms();
private:
	//Currently not used and unnecessary
	void SetMapDimensions(int xSize, int ySize) { xDimension = xSize; yDimension = ySize; }
	void SetRoomDimensions(int xSize, int ySize) { xRoomDimension = xSize; yRoomDimension = ySize; }


};

#endif // !PROCEDURALMAPMANAGER_H
