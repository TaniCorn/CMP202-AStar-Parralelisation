//////////
//////////CellularAutomata.h
//////////Written by Tanapat Somrid 
/////////Imported from a previous C# project 'Treasure Hunters', Massive thanks to Sebastian Lague's video 'https://www.youtube.com/watch?v=v7yyZZjF1z4'
//////// Most Recent Update 08/04/22
//////// Most Recent change: Changing source code from C# to C++, incorporating my RandomBase10 class

#pragma once
#ifndef CELLULARAUTOMATA_H
#define CELLULARAUTOMATA_H

#include "RandomBase10.h"

//Will need to incorporate the int map into Pathfinding map and turn all this information into node information
class CellularAutomata
{
public:
	const int width = 1000;
	const int height = 1000;
	int** map;
	int randomFillPercent = 45;
	int mapSmoothing = 3;
	void GenerateMap();
	void RandomFillMap();
	void SmoothingMap();
	int GetSurroundingWallCount(int gridX, int gridY);
};

#endif // !CELLULARAUTOMATA_H
