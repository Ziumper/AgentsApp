#pragma once
#include <random>
#include <random>


class Randomizer {
private:
	int mMin;
	int mMax;
	std::uniform_int_distribution<> distributor;
	std::mt19937 generator;
public:
	Randomizer(int min, int max);
	std::vector<int> GetEvenDistribute(int amount);
	int GetEvenRandomNumber();
};

class RealRandomizer {
private:
	double mMin;
	double mMax;
	std::uniform_real_distribution<> distributor;
	std::mt19937 generator;
public:
	RealRandomizer(double mMin, double mMax);
	std::vector<double> GetEventDistribute(double amount);
	double GetEvenRandomNumber();
};
