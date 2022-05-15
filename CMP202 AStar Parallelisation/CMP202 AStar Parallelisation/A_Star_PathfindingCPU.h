//////////
//////////A Star Pathfinding CPU files //Derived from CMP201 Project A* Segmented.
//////////This version of A* is a parallelised version of the previous CMP201 project, with many fixes to the original code and improvements. Having done this 'Segmented' version of A* I have heard of bidirectional A*. I would like to develop that as that is probably much more idea for a graph based system/one where the 'Route' nodes cannot be supplied. Aka a small room.
// ////// This segmented version is ideal for many 'Rooms' that are traversable. Not so much for 1 grid map full of obstacles - it will simply work as normal then(slighlty slower for preliminary checks)
//////////Written by Tanapat Somrid 
/////////Starting 19/04/2022
//////// Most Recent Update 14/05/2022
//////// Most Recent change: Final Sprint changes
//////// 
//Current Bug List:
//Will cause a GetParent() Error - When setting a Route Node as the Target Node

#pragma once

#ifndef A_STAR_PATHFINDINGCPU_H
#define A_STAR_PATHFINDINGCPU_H

#include <stack>
#include <queue>
#include "Rooms.h"
#include "VectorPositions.h"

struct ReverseComparator {
public:
	bool operator ()(const Node* a, const Node* b) const {
		//If they have the same fCost, they could be the same return if they are or not. 
		//If they are the same, it won't insert it. If they are not, it will be inserted at the current location. 
		//This is needed to prevent non duplicates from not being inserted.
		if (a->GetFCost() == b->GetFCost())
		{
			return a < b;
		}
		//If they have the exact same position they are the same so therefore get rid of one
		if (a->xPosition == b->xPosition)
		{
			if (a->yPosition == b->yPosition)
			{
				return a < b;
			}
		}
		//Repositioning based on fCost
		return (a->GetFCost() < b->GetFCost());
	}
};

/// <summary>
/// Base A*. Provides functionality and data that is absolutely required. Does not contain the A* algorithm itself.
/// <para>Some functions are parallelised already</para>
/// </summary>
class Base_A_Star_PathfindingCPU {
public:
	int threadsToUse = std::thread::hardware_concurrency();
	Node* target;//End node
	Node* root;//Start node

	std::vector<Room*> rooms;//Rooms we're allowed to traverse
	#pragma region FUNCTIONS_TO_CALL
	/// <summary>
	/// Set the rooms that the agent can travel through and search.
	/// </summary>
	void SetTraversableRooms(std::vector<Room*> rooms) {
		this->rooms = rooms;
	}

	/// <summary>
	/// Resets any 'Path' nodes back to their default node
	/// </summary>
	void ResetRoute();
	/// <summary>
	/// Find a path using the position of self and target
	/// </summary>
	void FindPath(Vector2<int> startPos, Vector2<int> endPos) {
		ResetRoute();
		
		//Parallelised
		if (!SetUpStartAndEndNodes(startPos, endPos)) {
			std::cout << "STARTNODEENDNODE FAILED!" << std::endl;
			return;
		}
		if(!AStarAlgorithm())
			std::cout << "PATHFINDING FAILED!" << std::endl;
	};	
	void FindPath(int xStart,int yStart,int xEnd, int yEnd) {
		ResetRoute();
		Vector2<int> startPos = Vector2<int>(xStart, yStart);
		Vector2<int> endPos = Vector2<int>(xEnd, yEnd);

		//Parallelised
		if (!SetUpStartAndEndNodes(startPos, endPos)) {
			std::cout << "STARTNODEENDNODE FAILED!" << std::endl;
			return;
		}
		if (!AStarAlgorithm())
			std::cout << "PATHFINDING FAILED!" << std::endl;
	};
	/// <summary>
	/// Changes any 'Free' nodes - that are in a recursive 'target.parent' node - to 'Path' nodes
	/// </summary>
	void PrintRoute();
	#pragma endregion


	/// <summary>
	/// Change a node at the given location to be a different type
	/// </summary>
	void EditNode(Vector2<int> nodePos, NodeType typeToChangeTo);

	//DEBUGGING
	/// <summary>
	/// For debugging purposes. Prints gCost, fCost, and position of a node.
	/// </summary>
	//void PrintNode(Node* n) {
	//	std::cout << "GCost:" << n->GetGCost() << " | FCost:" << n->GetFCost() << " | Position:" << n->position << std::endl;
	//}


protected:
	Room* startRoom;//Room that the start node is in


	/// <summary>
	/// Finds the corresponding nodes of each position and assigns them to root and target. 
	/// <para>Also sets up startRoom.</para>
	/// </summary>
	virtual bool SetUpStartAndEndNodes(Vector2<int> startPos, Vector2<int> endPos);

	/// <summary>
	/// A star algorithm
	/// </summary>
	virtual bool AStarAlgorithm() = 0;

	/// <summary>
	/// Finds the corresponding node at the coordinates given. Searches all available rooms.
	/// </summary>
	Node* GetNodeFromPosition(Vector2<int> position);

	/// <summary>
	/// Checks if the position is within room bounds
	/// </summary>
	static bool IsNodeInRoom(const RoomStruct& nm, const Vector2<int> position);
private:
	/// <summary>
	///	If the nodes position exists in the room, return the node.
	/// <para>Helper function for SetUpStartAndEndNodes()</para>
	/// </summary>
	Node* FindNodeInRoom(Vector2<int> pos, Room* rm);


};

/// <summary>
/// Extends upon Base A* to provide intended functionality.
/// <para>Is parallelised by the CPU to speed up pathfinding process</para>
/// </summary>
class A_Star_Pathfinding_Defined_SegmentedCPU : public Base_A_Star_PathfindingCPU {
public:
	A_Star_Pathfinding_Defined_SegmentedCPU() {};
	~A_Star_Pathfinding_Defined_SegmentedCPU() {
	};


private:
	bool AStarAlgorithm() override;
	bool MultiThreadedPathfinding(std::queue<Node*>* targetNodes);
	/// <summary>
	/// Searches through the rooms to find a path towards the target in room format
	/// </summary>
	/// <returns></returns>
	std::stack<RoomStruct*> BruteForcePathfindMaps();
	/// <summary>
	/// Find the path for the route nodes connecting the rooms from the BruteForcePathfindMaps function
	/// </summary>
	/// <param name="mapRoute"></param>
	/// <returns></returns>
	std::queue<Node*> FindRouteNodePaths(std::stack<RoomStruct*> mapRoute);

	/// <summary>
	/// Default a star to pathfind basic routes.
	/// </summary>
	/// <param name="startNode"></param>
	/// <param name="endNode"></param>
	/// <returns></returns>
	bool DefaultAStar(Node* startNode, Node* endNode);
	void CheckNeighbours(Node* node, Node* targetNode, std::set<Node*, ReverseComparator>* open, std::set<Node*>* closed);
	//bool DefaultAStar(Node* startNode, Node* endNode, int threads);
	//void CheckNeighbours(Node* node, int neighbourNum, Node* targetNode, std::set<Node*, ReverseComparator>* open, std::set<Node*>* closed);
	//void TestCheck(int i);

	/// <summary>
	/// Find the correct route nodes to use for pathfinding through the given rooms
	/// </summary>
	/// <param name="mapRoute"></param>
	/// <returns></returns>
	Node* FindRouteNode(std::stack<RoomStruct*>& mapRoute);
};


#include <iostream>
#include <fstream>

struct ExportData{
	std::string areaName;
	float timings;
};
class ExportBenchmark {
public:
	static void AddBenchmarkingInfo(ExportData ed) {
		if (current->size() >= 1600)
		{
			std::cout << "1600 threads reached. Switch to next threads\n";
			return;
		}
		current->push_back(ed);
	}
	static void ExportBenchmarkingInfo() {
		Exporting("Benchmarking1.txt", data, 1);
		Exporting("Benchmarking2.txt", data2, 2);
		Exporting("Benchmarking4.txt", data4, 4);
		Exporting("Benchmarking8.txt", data8, 8);
		Exporting("Benchmarking16.txt", data16, 16);
	
		std::cout << "Data Exported \n";
	}
	static void Switch(int i) {
		switch (i)
		{
		case 1:
			current = &data;
			break;
		case 2:		
			current = &data2;
			break;
		case 4:		
			current = &data4;
			break;
		case 8:		
			current = &data8;
			break;
		case 16:
			current = &data16;
			break;
		default:
			return;
			break;
		}
		std::cout << "NOW USING " << i << " THREADS \n";
	}
	static std::vector<ExportData>* current;
	static std::vector<ExportData> data;
	static std::vector<ExportData> data2;
	static std::vector<ExportData> data4;
	static std::vector<ExportData> data8;
	static std::vector<ExportData> data16;
private:
	static void Exporting(std::string fileName, std::vector<ExportData> dataSet, int threads) {
		if (dataSet.empty())
		{
			return;
		}
		std::ofstream file;
		file.open(fileName);
		file << threads << "\n";
		for (auto da : dataSet) {

			file << da.areaName << "," << da.timings << "\n";
		}
		file.close();
	}
};


////WARNING: This class with the implementation of it in DefaultAStar has a deadlock problem. The only way I can think to solve it is with a semaphore.
//class ThreadPool
//{
//public:
//	std::mutex taskLock;
//	std::condition_variable condVar;
//	bool finish;
//	std::queue <std::function <void(void)>> tasks;
//	std::vector <std::thread> workerThreads;
//	bool completedSection = true;
//	bool completedCheck = true;
//
//	ThreadPool(int threads) : finish(false)
//	{
//		// Create the specified number of threads
//		workerThreads.reserve(threads);
//		for (int i = 0; i < threads; ++i)
//			workerThreads.emplace_back(std::bind(&ThreadPool::threadEntry, this, i));
//	}
//
//	~ThreadPool()
//	{
//		{
//			// Unblock any threads and tell them to stop
//			std::unique_lock <std::mutex> l(taskLock);
//
//			finish = true;
//			condVar.notify_all();
//		}
//
//		// Wait for all threads to stop
//		for (auto& thread : workerThreads)
//			thread.join();
//	}
//
//	void doJob(std::function <void(void)> func)
//	{
//		// Place a job on the queu and unblock a thread
//		std::unique_lock <std::mutex> l(taskLock);
//		completedSection = false;
//		completedCheck = false;
//		tasks.emplace(std::move(func));
//		condVar.notify_one();
//	}
//
//private:
//
//	void threadEntry(int i)
//	{
//		std::function <void(void)> job;
//
//		while (true)
//		{
//			bool completeTasks = completedCheck;
//			{
//				std::unique_lock <std::mutex> lock(taskLock);
//
//				while (!finish && tasks.empty())
//					condVar.wait(lock);
//
//				if (tasks.empty())
//				{
//					// No jobs to do and we are shutting down
//					return;
//				}
//				job = std::move(tasks.front());
//				tasks.pop();
//				if (tasks.empty())
//				{
//					completedCheck = true;
//					completeTasks = true;
//				}
//
//			}
//
//			// Do the job without holding any locks
//			job();
//			if (completedCheck && completeTasks)
//			{
//				completedSection = true;
//			}
//		}
//
//	}
//
//
//};

#endif // !A_STAR_PATHFINDINGCPU_H
