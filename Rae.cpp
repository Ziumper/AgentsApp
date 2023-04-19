#include "Rae.h"
#include <vector>
#include <random>

namespace Rae {
	
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

	void MonteCarlo::RunCycle(Cycle cycle)
	{
		logger->AddLog("Starting cycle ", cycle.round);


		Agent firstSupplier = cycle.agents[0];
		firstSupplier.trust = beginTrustMesaure;

		logger->AddLog("Initialized starting trust for first recipent agent: V_0:", beginTrustMesaure);

		logger->AddLog("Choosing the recipents");
		
		Randomizer randomizer = Randomizer(kMin,kMax);

		
		
	}

	void MonteCarlo::Start()
	{
		logger->AddLog("Starting Monte Carlo simulation");

		std::vector<Cycle> cycles;

		for (int t = 0; t < cyclesAmount; t++) {
			Cycle cycle = Cycle(t, agentsAmount);
			logger->AddLog("Created cycle: ", cycle.round);
			RunCycle(cycle);

			
			cycles.push_back(cycle);
		}

		logger->AddLog("Monte carlo simulation is done");
	}

	Cycle::Cycle(int roundNumber, int agentsAmount)
	{
		round = roundNumber;
		
		for (int j = 0; j < agentsAmount; j++) {
			Agent agent = Agent();
			agent.number = j;
			agents.push_back(agent);
		}
	}

}