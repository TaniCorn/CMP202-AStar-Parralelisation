
//////////
//////////Pathfinding Visualisation files
//////////Written by Tanapat Somrid 
/////////Starting 15/04/2022
//////// Most Recent Update 20/04/2022
//////// Most Recent change: Parallelised Update Object function

//TODO: Allow a slow mode and implement new node types(open set closed set)
#pragma once
#ifndef PATHFINDINGVISUALISATION_H

#define PATHFINDINGVISUALISATION_H

#include "Input.h"
#include "SFML\Graphics.hpp"
#include "VectorPositions.h"
#include "ProceduralMapManager.h"
#include "A_Star_PathfindingCPU.h"
#include "A_Star_PathfindingGPU.h"

class PathfindingVisualisation
{
public:
	void Init(Input* in, sf::RenderWindow* wind, sf::View* vie);

	void Update();
	void HandleInput();
	void Render();


	void UpdateObjects(Vector2<int> mapArray, ProceduralMapManager* pmm, std::vector<sf::RectangleShape>* drawableShapes);
	void PathfindUpdate(Vector2<int> start, Vector2<int> end);
	sf::RenderWindow* window;
	sf::View* view;
	Input* input;

    ProceduralMapManager pmm;


	std::vector<sf::RectangleShape> rectDraw;
	Vector2<int> position = Vector2<int>(0, 0);
	sf::Vector2f viewCenter;
	sf::Vector2f addViewCenter;


	A_Star_Pathfinding_Defined_SegmentedGPU* pathfindingAgent_CPU;
	//A_Star_Pathfinding_Defined_Segmented* pathfindingAgent_NORMAL;


	//User Interaction
	bool startSet = false; Vector2<int> startPosition;
	bool endSet = false; Vector2<int> endPosition;
	bool toggle = false;
	bool editMap = false;
};

#endif // !PATHFINDINGVISUALISATION
