#include "ProceduralMapManager.h"
#include <vector>
#include <algorithm>
#include <thread>

#include <chrono>


void ProceduralMapManager::GenerateMapGrid()
{
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
	


	//Changes how many threads gets used depending on data size
	int threadDivide = 1;
	if (std::thread::hardware_concurrency() > 8)
	{
		//threadDivide = floor(std::thread::hardware_concurrency() / (2 + ((xDimension * yDimension) > 5000) + ((xDimension * yDimension) < 1000) + ((xDimension * yDimension) < 100) ) );
		if ((xDimension * yDimension) >= 10)
		{
			threadDivide = 4;
		}
		else {
			threadDivide = 8;
		}
		if ((xDimension * yDimension) >= 1000)
		{
			threadDivide = 2;
		}
		if ((xDimension * yDimension) >= 10000)
		{
			threadDivide = 1;
		}		
	}


	for (int i = 0; i < std::thread::hardware_concurrency() / threadDivide; i++)
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
				//Example of 5*10 = 50 map size; //Start from currentRoomNumber = 0
				/*
				//After some testing, unable to determine which is faster.
				*/
				//int xDim = currentRoomNumber % roomXAmount;//(0,1,2,3,4,,0,1,2,3...)
				//int yDim = floor(currentRoomNumber / roomXAmount);//(0,0,0,0,1,1,1...)				
				int xDim = floor(currentRoomNumber / roomYAmount);//(0,1,2,3,4,,0,1,2,3...)
				int yDim = currentRoomNumber % roomYAmount;//(0,0,0,0,1,1,1...)
				//This will access the array x along(TODO: Test - swap around to access y along)
				Room* currentRoom = &roomsInMap[xDim][yDim];
				currentRoomNumber++;
				lock.unlock();

				currentRoom->GenerateAutomataRoom(Vector2<int>(xRoomDimension, yRoomDimension), Vector2<int>(xDim * xRoomDimension, yDim * yRoomDimension));
			}
			}));
	}
	//////Debug
//for (int x = 0; x < roomXAmount; x++)
//{
//	for (int y = 0; y < roomYAmount; y++)
//	{
//		roomsInMap[x][y].GenerateAutomataMap(Vector2<int>(xRoomDimension, yRoomDimension), Vector2<int>(x * xRoomDimension, y * yRoomDimension));
//	}
//}

	std::for_each(threads.begin(), threads.end(), [](std::thread& t) 
		{
		t.join();
		});

}

void ProceduralMapManager::ConnectRooms()
{
	//Currently only works with rooms bigger than a 2x2
	//Horizontal Connected Route Nodes
	int horizontal_RightX = xRoomDimension - 2;
	int horizontal_LeftX = 2;
	int horizontal_Y = yRoomDimension / 2;	
	//Vertically Connected Route Nodes
	int vertical_X = xRoomDimension / 2;
	int vertical_BottomY = yRoomDimension - 2;
	int vertical_TopY = 2;

	//Loops entire map except for edges
	for (int y = 0; y < yDimension - 1; y++)
	{
		for (int x = 0; x < xDimension - 1; x++)
		{
			//Links this room and the one to the right
			TeleportNode* node1 = roomsInMap[x][y].AddRouteNode(horizontal_RightX, horizontal_Y);
			TeleportNode* node2 = roomsInMap[x+1][y].AddRouteNode(horizontal_LeftX, horizontal_Y);

			roomsInMap[x][y].DualLinkRouteNodes(node1, node2);
			if (failsafe)
			{
				for (int i = 1; i < (xRoomDimension / 2); i++)
				{
					roomsInMap[x][y].nodes[horizontal_RightX - i][horizontal_Y].nodeType = Free;
					roomsInMap[x+1][y].nodes[horizontal_LeftX + i][horizontal_Y].nodeType = Free;
				}
			}

			//Links this room and the one below this one
			node1 = roomsInMap[x][y].AddRouteNode(vertical_X, vertical_BottomY);
			node2 = roomsInMap[x][y + 1].AddRouteNode(vertical_X, vertical_TopY);

			roomsInMap[x][y].DualLinkRouteNodes(node1, node2);
			if (failsafe)
			{
				for (int i = 1; i < (yRoomDimension / 2); i++)
				{
					roomsInMap[x][y].nodes[vertical_X][vertical_BottomY - i].nodeType = Free;
					roomsInMap[x][y+1].nodes[vertical_X][vertical_TopY + i].nodeType = Free;
				}
			}
		}
	}
}


//std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
//std::cout << ms << std::endl;