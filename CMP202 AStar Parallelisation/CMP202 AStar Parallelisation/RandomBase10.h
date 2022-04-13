//////////
//////////RandomBase10.h
//////////Written by Tanapat Somrid 
/////////Imported from CMP208 project
//////// Most Recent Update: 13/04/22
//////// Most Recent change: Ignore last comment, unknown why I removed the random device function for EngineRandomNumber. That is now back and EngineRandomNumber works as intended and is thread safe. Also now has default parameters for the sequence generators.

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
	/// <summary>
	/// Fast random number generator. NOT THREAD SAFE
	/// </summary>
	static int QuickRandomNumber(int lowerBound, int upperBound);//Uses rand
	/// <summary>
	/// Fast random sequence generator. NOT THREAD SAFE
	/// <para>Currently only tested with values between 1000 and 9999</para>
	/// </summary>
	static std::vector<int> QuickRandomSequence(int lowerBound = 1000, int upperBound = 9999);//Uses rand
	/// <summary>
	/// Thread safe random number generator
	/// </summary>
	static int EngineRandomNumber(int lowerBound, int upperBound);// uses mt19937 and uniform distribution
	/// <summary>
	/// Thread safe random sequence generator
	///<para>Currently only tested with values between 1000 and 9999</para>
	/// </summary>
	static std::vector<int> EngineRandomSequence(int lowerBound = 1000, int upperBound = 9999);// uses mt19937 and uniform distribution
};

#endif // !RANDOMBASE10_H
