
//////////
//////////Pathfinding Visualisation files
////////// Using SFML, this class allows us to visualise and interact with the map
//////////Written by Tanapat Somrid 
/////////Starting 15/04/2022
//////// Most Recent Update 07/05/2022
//////// Most Recent change: Cleanup

//TODO: Allow a slow mode and implement new node types(open set closed set)
	//TODO: There is a bigger delay in input/output(Pathfinding takes much longer? perhaps) When the distance is larger in terms of maps. 
	//For example, the time difference for map[0][0].node[0][0] - map[1][0].node[99][0] is almost the same as map[0][0].node[99][0] - map[1][0].node[0][0].
	//But the time difference dramatically increases for map[0][0].node[0][0] - map[2][0].node[0][0]
	//Basically the more maps away we are the bigger the time difference

//TODO: Allow placement of Route nodes? Use the same concept of left shift and apply that to left ctrl?
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
	sf::RenderWindow* window;
	sf::View* view;
	Input* input;

	void Init(Input* in, sf::RenderWindow* wind, sf::View* vie);
	void Update();
	void HandleInput();
	void Render();


	void UpdateObjects(Vector2<int> mapArray, ProceduralMapManager* pmm, std::vector<sf::RectangleShape>* drawableShapes);
	void PathfindUpdate(Vector2<int> start, Vector2<int> end);


    ProceduralMapManager pmm;
	A_Star_Pathfinding_Defined_SegmentedCPU* pathfindingAgent_CPU;
	//A_Star_Pathfinding_Defined_Segmented* pathfindingAgent_NORMAL;

	//Visualisations
	std::vector<sf::RectangleShape> rectDraw;
	Vector2<int> position = Vector2<int>(0, 0);
	sf::Vector2f viewCenter;
	sf::Vector2f addViewCenter;




	//User Interaction
	bool startSet = false; Vector2<int> startPosition;
	bool endSet = false; Vector2<int> endPosition;
	bool toggle = false;
	bool editMap = false;

	//Benchmarking
	bool automatic = false;

};

#endif // !PATHFINDINGVISUALISATION
