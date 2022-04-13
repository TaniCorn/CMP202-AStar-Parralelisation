//////////
//////////CellularAutomata.h
//////////Written by Tanapat Somrid 
/////////Imported from a previous C# project 'Treasure Hunters', Massive thanks to Sebastian Lague's video 'https://www.youtube.com/watch?v=v7yyZZjF1z4'
//////// Most Recent Update 13/04/22
//////// Most Recent change: Changed the random function to use Engine

#pragma once
#ifndef CELLULARAUTOMATA_H
#define CELLULARAUTOMATA_H

#include "RandomBase10.h"

//Will need to incorporate the int map into Pathfinding map and turn all this information into node information
class CellularAutomata
{
public:
	CellularAutomata() {};
	~CellularAutomata() {
		for (int i = 0; i < width - 1; ++i)
		{
			delete[] map[i];
		}
	};
	int width = 100;
	int height = 100;
	int** map;
	int randomFillPercent = 45;
	int mapSmoothing = 5;
	void GenerateMap(int xDimension, int yDimension);
	void RandomFillMap();
	void SmoothingMap();
	int GetSurroundingWallCount(int gridX, int gridY);
};

#endif // !CELLULARAUTOMATA_H
