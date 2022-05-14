#include "CellularAutomata.h"
#include <iostream>

void CellularAutomata::GenerateMap(int xDimension, int yDimension)
{
    width = xDimension; height = yDimension;
    int** tempMap = new int* [width];
    for (int i = 0; i < height; i++)
    {
        tempMap[i] = new int[height];
    }
    map = tempMap;
    RandomFillMap();

    for (int i = 0; i < mapSmoothing; i++)
    {
        SmoothingMap();
    }
}

void CellularAutomata::RandomFillMap()
{

    //Randomly allocates map grid 1's and 0's
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            //Perimeter of map is 1
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1)
            {
                map[x][y] = 1;
            }
            else
            {
                map[x][y] = (RandomBase10::EngineRandomNumber(0, 100) < randomFillPercent) ? 1 : 0;
            }
        }
    }
}

void CellularAutomata::SmoothingMap()
{
    //Smooths map
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            int neighbourWallTiles = GetSurroundingWallCount(x, y);
            //These values work quite well however they can be changed to produce vastly different results.
            if (neighbourWallTiles > 4)
            {
                map[x][y] = 1;
            }
            else if (neighbourWallTiles < 4)
            {
                map[x][y] = 0;
            }
        }

    }
}

int CellularAutomata::GetSurroundingWallCount(int gridX, int gridY)
{
    int wallCount = 0;
    //Checks a 3 by 3 grid around the specified cell location
    for (int neighbourX = gridX - 1; neighbourX <= gridX + 1; neighbourX++)
    {
        for (int neighbourY = gridY - 1; neighbourY <= gridY + 1; neighbourY++)
        {
            //If we're on perimeter of map wallcount++ otherwise:
            if (neighbourX >= 0 && neighbourX < width && neighbourY >= 0 && neighbourY < height)
            {
                //Being honest, no idea how this works
                if (neighbourX != gridX || neighbourY != gridY)
                {
                    wallCount += map[neighbourX][neighbourY];
                }
            }
            else
            {
                wallCount++;
            }

        }
    }

    return wallCount;
}