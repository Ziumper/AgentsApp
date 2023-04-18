#include "Rae.h"
#include <vector>
#include <random>

namespace Rae {
	/// <summary>
	/// Returns the array with numbers evenly distributed
	/// </summary>
	/// <returns></returns>
	std::vector<int> Randomizer::GetEvenDistribute()
	{
		int end = *mMax.get();
		int start = *mMin.get();

		return GetEvenDistribute(end - start);
	}

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

	void Rtbs::StartMonteCarlo()
	{
		logger->AddLog("Starting Monte Carlo simulation!");

		for (int i = 0; i < *cycles; i++) {
			RunCycle(i);
		}

		logger->AddLog("Done");
	}

	void Rtbs::RunCycle(int cycleNumber)
	{
		logger->AddLog("Running cycle: ", cycleNumber);

		logger->AddLog("Creating agents");
		std::vector<Agent> listOfAgents;
		
		Agent *agent = new Agent();
		agent->number = 7;
		agent->trust = 20;

		listOfAgents.push_back(*agent);

		//listOfAgents->push_back(agent);
		//for (int i = 0; i < *agentsAmount; i++) {
		//	Agent agent;
		//	agent.number = std::make_unique<int>(i);
		//	
		//	if (i == 0) {
		//		//double beginnerTrust = *beginTrustMesaure;
		//		//agent.trust = std::make_unique<double>(beginnerTrust);
		//	}

		//	listOfAgents->push_back(agent);
		//}

		logger->AddLog("Created agents: ", *agentsAmount);
	}
}