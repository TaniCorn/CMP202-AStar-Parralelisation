#include "ProceduralMapManager.h"
#include <vector>

#include <chrono>
#include <iostream>
//std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//std::cout << ms << std::endl;

void ProceduralMapManager::GenerateMapGrid()
{
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	int	roomXAmount = xDimension;//In case I want to change it...
	int roomYAmount = yDimension;

	//Create Rooms for map
	Room** f = new Room * [roomXAmount];
	for (int i = 0; i < roomXAmount; i++)
	{
		f[i] = new Room[roomYAmount];
	}
	roomsInMap = f;

	//Initialising a farming pattern
	std::mutex lock;
	std::vector<std::thread> threads;

	int total = roomXAmount * roomYAmount;//Total amount of rooms
	int currentRoomNumber = 0;

	//Generates automata rooms and links the neighbours with CPU Parallelisation Farming
	#pragma region GENERATING_ROOMS
	for (int i = 0; i < threadsToUse; i++)
	{
		threads.push_back(std::thread([&]() {

			while (currentRoomNumber < total)
			{
				lock.lock();
				if (currentRoomNumber == total)
				{
					lock.unlock();
					return;
				}
				srand(i);
		
				//After some testing, unable to determine which is faster.
				//This is probably due to this being CPU parallelisation which doesn't have as big of an emphasis on data access, cache as GPU does
	
				//int xDim = currentRoomNumber % roomXAmount;//(0,1,2,3,4,,0,1,2,3...)
				//int yDim = floor(currentRoomNumber / roomXAmount);//(0,0,0,0,1,1,1...)				
				int xDim = floor(currentRoomNumber / roomYAmount);//(0,1,2,3,4,,0,1,2,3...)
				int yDim = currentRoomNumber % roomYAmount;//(0,0,0,0,1,1,1...)
				Room* currentRoom = &roomsInMap[xDim][yDim];
				currentRoomNumber++;
				lock.unlock();

				currentRoom->GenerateAutomataRoom(xRoomDimension,yRoomDimension, xDim* xRoomDimension, yDim * yRoomDimension);
				currentRoom->LinkNeighbours(*currentRoom);
			}
			}));
	}
	#pragma endregion

	std::for_each(threads.begin(), threads.end(), [](std::thread& t) 
		{
		t.join();
		});
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "ProceduralMapManager::GenerateMapGrid() takes - " << ms << "ms" << std::endl;
	
}

void ProceduralMapManager::ConnectRooms()
{
	//WARNING: Currently only works with maps set bigger or equal to a 2x2
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

	//Horizontal Connected Route Nodes - These will be on the middle left and right sides of the map
	int horizontal_RightX = xRoomDimension - 2;//2 nodes away from right edge
	int horizontal_LeftX = 2; // 2 nodes away from left edge
	int horizontal_Y = yRoomDimension / 2;	//half way down the map

	//Vertically Connected Route Nodes - these will be on the middle top and bottom sides of the map
	int vertical_X = xRoomDimension / 2; //halfway across the map
	int vertical_BottomY = yRoomDimension - 2; // 2 nodes away from bottom edge
	int vertical_TopY = 2; // 2 nodes away from top edge

	//Loops all Rooms in Map except for bottom right room
	for (int y = 0; y < yDimension - 1; y++)
	{
		for (int x = 0; x < xDimension - 1; x++)
		{
			#pragma region HORIZONTAL_LINKING

			//Links this room and the one to the right of it
			TeleportNode* node1 = roomsInMap[x][y].SpawnRouteNode(horizontal_RightX, horizontal_Y);
			TeleportNode* node2 = roomsInMap[x+1][y].SpawnRouteNode(horizontal_LeftX, horizontal_Y);
			roomsInMap[x][y].DualLinkRouteNodes(node1, node2);

			//Will make sure that these two Route nodes have a Free path all the way to the center of their room
			if (failsafe)
			{
				for (int i = 1; i < (xRoomDimension / 2); i++)
				{
					roomsInMap[x][y].nodes[horizontal_RightX - i][horizontal_Y].nodeType = Free;
					roomsInMap[x+1][y].nodes[horizontal_LeftX + i][horizontal_Y].nodeType = Free;
				}
			}

			//Adds necessary information for pathfinding
			roomsInMap[x][y].AddNeighbouringRoom(&roomsInMap[x + 1][y]);
			roomsInMap[x+1][y].AddNeighbouringRoom(&roomsInMap[x][y]);
			roomsInMap[x][y].AddRouteNode(node1);
			roomsInMap[x+1][y].AddRouteNode(node2);
			#pragma endregion

			#pragma region VERTICAL_LINKING
			//Links this room and the one below this one
			node1 = roomsInMap[x][y].SpawnRouteNode(vertical_X, vertical_BottomY);
			node2 = roomsInMap[x][y + 1].SpawnRouteNode(vertical_X, vertical_TopY);
			roomsInMap[x][y].DualLinkRouteNodes(node1, node2);
			
			//Will make sure that these two Route nodes have a Free path all the way to the center of their room
			if (failsafe)
			{
				for (int i = 1; i < (yRoomDimension / 2); i++)
				{
					roomsInMap[x][y].nodes[vertical_X][vertical_BottomY - i].nodeType = Free;
					roomsInMap[x][y+1].nodes[vertical_X][vertical_TopY + i].nodeType = Free;
				}
			}

			//Adds necessary information for pathfinding
			roomsInMap[x][y].AddNeighbouringRoom(&roomsInMap[x][y+1]);
			roomsInMap[x][y+1].AddNeighbouringRoom(&roomsInMap[x][y]);
			roomsInMap[x][y].AddRouteNode(node1);
			roomsInMap[x][y+1].AddRouteNode(node2);
			#pragma endregion
		}
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "ProceduralMapManager::ConnectRooms() takes - " << ms << "ms" << std::endl;
}


