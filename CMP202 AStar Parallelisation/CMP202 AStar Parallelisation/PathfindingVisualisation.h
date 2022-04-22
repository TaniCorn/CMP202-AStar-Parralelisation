
//////////
//////////Pathfinding Visualisation files
//////////Written by Tanapat Somrid 
/////////Starting 15/04/2022
//////// Most Recent Update 20/04/2022
//////// Most Recent change: Parallelised Update Object function
#pragma once
#ifndef PATHFINDINGVISUALISATION_H

#define PATHFINDINGVISUALISATION_H

#include "Input.h"
#include "SFML\Graphics.hpp"
#include "VectorPositions.h"
#include "ProceduralMapManager.h"
#include "A_Star_PathfindingCPU.h"

class PathfindingVisualisation
{
public:
	void Init(Input* in, sf::RenderWindow* wind, sf::View* vie);

	void Update();
	void HandleInput();
	void Render();


	void UpdateObjects(Vector2<int> mapArray, ProceduralMapManager* pmm, std::vector<sf::RectangleShape>* drawableShapes);
	sf::RenderWindow* window;
	sf::View* view;
	Input* input;

    ProceduralMapManager pmm;


	std::vector<sf::RectangleShape> rectDraw;
	Vector2<int> position = Vector2<int>(0, 0);
	sf::Vector2f viewCenter;
	sf::Vector2f addViewCenter;


	A_Star_Pathfinding_Defined_SegmentedCPU* pathfindingAgent;
};

#endif // !PATHFINDINGVISUALISATION
