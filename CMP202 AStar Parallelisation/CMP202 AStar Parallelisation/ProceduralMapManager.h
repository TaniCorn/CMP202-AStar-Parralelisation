

//////////
//////////Procedural Map Manager files
//////////Written by Tanapat Somrid 
/////////Starting 11/04/2022
//////// Most Recent Update 13/04/2022
//////// Most Recent change: Changed xDimension to reflect number of rooms rather than span of total nodes
#pragma once

#ifndef PROCEDURALMAPMANAGER_H
#define PROCEDURALMAPMANAGER_H
#include "PathfindingMap.h"
#include <thread>
#include <mutex>
#include <math.h>
#include <algorithm>

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

	Room** roomsInMap;

	void Init(int xMapSize, int yMapSize, int xRoomDimensions, int yRoomDimensions) { xDimension = xMapSize; yDimension = yMapSize; xRoomDimension = xRoomDimensions; yRoomDimension = yRoomDimensions; }
	void GenerateMapGrid();
	void SetMapDimensions(int xSize, int ySize) { xDimension = xSize; yDimension = ySize; }
	void SetRoomDimensions(int xSize, int ySize) { xRoomDimension = xSize; yRoomDimension = ySize; }
	int xDimension, yDimension;
	int xRoomDimension, yRoomDimension;
};

#endif // !PROCEDURALMAPMANAGER_H
