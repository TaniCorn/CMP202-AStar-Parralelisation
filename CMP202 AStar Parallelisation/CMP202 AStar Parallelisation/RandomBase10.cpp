#include "RandomBase10.h"
#include <thread>
int RandomBase10::LongRandomNumber()
{
	int randomSeed = rand() % 10000 + 1000;
	static std::hash<int> long_hash_;

	int randomNumber = long_hash_(randomSeed);
	return randomNumber;
}

std::vector<int> RandomBase10::LongRandomSequence()
{
	int randomNumber = LongRandomNumber();
	std::vector<int> sequence;
	sequence.push_back((randomNumber / 1000) % 10);
	sequence.push_back((randomNumber / 100) % 10);
	sequence.push_back((randomNumber / 10) % 10);
	sequence.push_back((randomNumber % 10));
	return sequence;
}



int RandomBase10::QuickRandomNumber(int lb, int ub)
{
	int randomNumber = rand() % ub + lb;
	
	return randomNumber;
}

std::vector<int> RandomBase10::QuickRandomSequence(int lb, int ub)
{
	int randomNumber = QuickRandomNumber(lb, ub);
	std::vector<int> sequence;
	sequence.push_back((randomNumber / 1000) % 10);
	sequence.push_back((randomNumber / 100) % 10);
	sequence.push_back((randomNumber / 10) % 10);
	sequence.push_back((randomNumber % 10));
	return sequence;
}

int RandomBase10::EngineRandomNumber(int lb, int ub)
{
	std::random_device randomDevice;//Gets a random device
	int seed = randomDevice();
	std::mt19937 gen(seed);
	std::uniform_int_distribution<> randomDistributer(lb, ub);

	return randomDistributer(gen);
}

std::vector<int> RandomBase10::EngineRandomSequence(int lb, int ub)
{
	int randomNumber = EngineRandomNumber(lb, ub);

	std::vector<int> sequence;
	sequence.push_back(((randomNumber / 1000) % 10)*10);
	sequence.push_back(((randomNumber / 100) % 10)*10);
	sequence.push_back(((randomNumber /10) %10)*10);
	sequence.push_back(((randomNumber % 10))*10);
	return sequence;
}


