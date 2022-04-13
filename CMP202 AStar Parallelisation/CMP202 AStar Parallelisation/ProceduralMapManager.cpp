#include "ProceduralMapManager.h"
#include <vector>
#include <algorithm>
#include <thread>

void ProceduralMapManager::GenerateMapGrid()
{
	int	roomXAmount = xDimension / xRoomDimension;
	int roomYAmount = yDimension / yRoomDimension;
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
	
	//////Debug
	//for (int x = 0; x < roomXAmount; x++)
	//{
	//	for (int y = 0; y < roomYAmount; y++)
	//	{
	//		roomsInMap[x][y].GenerateAutomataMap(Vector2<int>(xRoomDimension, yRoomDimension), Vector2<int>(x * xRoomDimension, y * yRoomDimension));
	//	}
	//}
	for (int i = 0; i < 4; i++)
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
				int xDim = currentRoomNumber % roomXAmount;//(0,1,2,3,4,,0,1,2,3...)
				int yDim = floor(currentRoomNumber / roomXAmount);//(0,0,0,0,1,1,1...)
				//This will access the array x along(TODO: Test - swap around to access y along)
				Room* currentRoom = &roomsInMap[xDim][yDim];
				currentRoomNumber++;
				lock.unlock();

				currentRoom->GenerateAutomataMap(Vector2<int>(xRoomDimension, yRoomDimension), Vector2<int>(xDim * xRoomDimension, yDim * yRoomDimension));
			}
			}));
	}

	std::for_each(threads.begin(), threads.end(), [](std::thread& t) 
		{
		t.join();
		});

}
