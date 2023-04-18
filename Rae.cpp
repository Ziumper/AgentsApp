#include "Rae.h"
#include <vector>
#include <random>

namespace Rae {
	/// <summary>
	/// Returns the array with numbers evenly distributed
	/// </summary>
	/// <returns></returns>
	std::vector<int> Randomizer::getEvenDistribute()
	{
		int end = *mMax.get();
		int start = *mMin.get();

		return getEvenDistribute(end - start);
	}

	std::vector<int> Randomizer::getEvenDistribute(int amount)
	{
		std::vector<int> randomNumbers = std::vector<int>();

		for (int i = 0; i < amount; i++) {
			int randomNumber = this->getEvenRandomNumber();
			randomNumbers.push_back(randomNumber);
		}

		return randomNumbers;
	}
	int Randomizer::getEvenRandomNumber()
	{
		int randomNumber = distributor(generator);
		return randomNumber;
	}

	Randomizer::Randomizer(int min, int max) :
		mMax(std::make_unique<int>(max)),
		mMin(std::make_unique<int>(min))
	{
		/// https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
		std::random_device random;  // Will be used to obtain a seed for the random number engine
		std::mt19937 generator(random()); // Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<> distributor(*mMin.get(), *mMax.get());

		this->generator = generator;
		this->distributor = distributor;
	};

	void Rtbs::StartMonteCarlo(RaeLogger* logger)
	{
		logger->AddLog("Starting Monte Carlo simulation!");

		Randomizer randomizer = Randomizer(*kMin.get(), *kMax.get());
		auto range = randomizer.getEvenRandomNumber();

		logger->AddLog("Done");
	}
}