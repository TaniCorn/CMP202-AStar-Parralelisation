//////////
//////////RandomBase10.h
//////////Written by Tanapat Somrid 
/////////Imported from CMP208 project
//////// Most Recent Update: 08/04/22
//////// Most Recent change: Added parameters to the EngineRandomNumber and QuickRandomNumber. And also found out my EngineRandomNumber kinda sucks :(

#pragma once
#ifndef RANDOMBASE10_H
#define RANDOMBASE10_H
#include <random>
#include <functional>
#include <ctime>
#include <vector>
class RandomBase10
{
public:
	
	RandomBase10() { srand(time(0));}

	static int LongRandomNumber();//Uses rand and hash
	static std::vector<int> LongRandomSequence();//Uses rand and hash
	static int QuickRandomNumber(int lowerBound, int upperBound);//Uses rand
	static std::vector<int> QuickRandomSequence();//Uses rand
	static int EngineRandomNumber(int lowerBound, int upperBound);// uses mt19937 and uniform distribution
	static std::vector<int> EngineRandomSequence();// uses mt19937 and uniform distribution
};

#endif // !RANDOMBASE10_H
