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

std::vector<int> RandomBase10::QuickRandomSequence()
{
	int randomNumber = QuickRandomNumber(1000,10000);
	std::vector<int> sequence;
	sequence.push_back((randomNumber / 1000) % 10);
	sequence.push_back((randomNumber / 100) % 10);
	sequence.push_back((randomNumber / 10) % 10);
	sequence.push_back((randomNumber % 10));
	return sequence;
}

int RandomBase10::EngineRandomNumber(int lb, int ub)
{
	//std::random_device randomDevice;//Gets a random device
	std::mt19937 gen(0);//This actually will give consistant results :(. It does not randomise very well in short spaces of time
	std::uniform_int_distribution<> randomDistributer(lb, ub);

	return randomDistributer(gen);
}

std::vector<int> RandomBase10::EngineRandomSequence()
{
	//std::random_device randomDevice;//Gets a random device
	std::mt19937 gen(time(0));
	std::uniform_int_distribution<> randomDistributer(1000, 9999);

	int randomNumber = randomDistributer(gen);
	std::vector<int> sequence;
	sequence.push_back(((randomNumber / 1000) % 10)*10);
	sequence.push_back(((randomNumber / 100) % 10)*10);
	sequence.push_back(((randomNumber /10) %10)*10);
	sequence.push_back(((randomNumber % 10))*10);
	return sequence;
}


