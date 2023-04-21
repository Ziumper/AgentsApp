#include <vector>
#include "Randomizer.h"

std::vector<int> Randomizer::GetEvenDistribute(int amount)
{
	std::vector<int> randomNumbers = std::vector<int>();

	for (int i = 0; i < amount; i++) {
		int randomNumber = this->GetEvenRandomNumber();
		randomNumbers.push_back(randomNumber);
	}

	return randomNumbers;
}
int Randomizer::GetEvenRandomNumber()
{
	int randomNumber = distributor(generator);
	return randomNumber;
}

Randomizer::Randomizer(int min, int max) :
	mMax(max),
	mMin(min)
{
	/// https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
	std::random_device random;  // Will be used to obtain a seed for the random number engine
	std::mt19937 generator(random()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distributor(mMin, mMax);

	this->generator = generator;
	this->distributor = distributor;
};

RealRandomizer::RealRandomizer(double mMin, double mMax)
{
	this->mMax = mMax;
	this->mMin = mMin;

	std::random_device random;  // Will be used to obtain a seed for the random number engine
	std::mt19937 generator(random()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> distributor(mMin, mMax);

	this->generator = generator;
	this->distributor = distributor;
}

std::vector<double> RealRandomizer::GetEventDistribute(double amount)
{
	std::vector<double> randomNumbers;

	for (int i = 0; i < amount; i++) {
		double randomNumber = this->GetEvenRandomNumber();
		randomNumbers.push_back(randomNumber);
	}

	return randomNumbers;
}

double RealRandomizer::GetEvenRandomNumber()
{
	double randomNumber = distributor(generator);
	return randomNumber;
}
