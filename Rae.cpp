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
			int randomNumber = this->GetEvenRandomNumber();
			randomNumbers.push_back(randomNumber);
		}

		return randomNumbers;
	}

	double RealRandomizer::GetEvenRandomNumber()
	{
		double randomNumber = distributor(generator);
		return randomNumber;
	}


	void MonteCarlo::RunCycle(Cycle *cycle)
	{
		logger->AddLog("Starting cycle: ", cycle->round);
		ChooseSuppilers(cycle);
	}

	void MonteCarlo::ChooseSuppilers(Cycle *cycle)
	{
		//recipent would be the one from round
		Agent *recipent = &cycle->agents[cycle->round];
		recipent->wasRecipent = true;

		logger->AddLog("Choosing the suppliers agents for cycle: ", cycle->round);
		Randomizer randomizer = Randomizer(kMin, kMax);
		int suppilersAmount = randomizer.GetEvenRandomNumber();

		int amountOfAgentsLeft = agentsAmount - 1;
		Randomizer supplierRandomizer = Randomizer(0, amountOfAgentsLeft); //amount of agents to choose from

		std::vector<int> cycleAgentRecipentsNumbers = supplierRandomizer.GetEvenDistribute(suppilersAmount); //distribute random from amount of agents to choose

		std::vector<Agent> suppilers;
		int choosenRecipentsCounter = 0;
		for (int& number : cycleAgentRecipentsNumbers) {

			//special case handle when we got recipent inside suppiler / shouldn't happen so often
			while (number == recipent->number) {
				number = supplierRandomizer.GetEvenRandomNumber();
			}

			Agent supplier = cycle->agents[number];
	
			SetServiceAvailiabilityForAgent(&supplier);
			SetServiceReceptionForAgent(&supplier);
			
			suppilers.push_back(supplier);
		}

		cycle->recipent = *recipent;
		cycle->suppilers = suppilers;

		logger->AddLog("Amount of recpients choosen: ", suppilersAmount);
	}

	void MonteCarlo::SetServiceAvailiabilityForAgent(Agent* agent)
	{
		RealRandomizer agentServiceRandomizer = RealRandomizer(0, 1);
		
		double serviceA = agentServiceRandomizer.GetEvenRandomNumber();
		double availiability = std::pow(serviceA, expoG);

		agent->serviceAvailiability = availiability;
	}

	void MonteCarlo::SetServiceReceptionForAgent(Agent* agent)
	{
		RealRandomizer agentServiceRandomizer = RealRandomizer(0, 1);

		double serviceA = agentServiceRandomizer.GetEvenRandomNumber();
		double reception = std::pow(serviceA, expoA);

		agent->serviceReception = reception;
	}

	
	void MonteCarlo::Start()
	{
		logger->AddLog("Starting Monte Carlo simulation");

		std::vector<Cycle> cycles;
		std::vector<Agent> agents;

		logger->AddLog("Creating agents: ",agentsAmount);
		for (int i = 0; i < agentsAmount; i++) {
			Agent agent = Agent();
			agent.number = i;
			agent.trust = beginTrustMesaure;
			agents.push_back(agent);
		}

		//preserve agents to next cycle
		std::vector<Agent> tempAgents = agents;

		logger->AddLog("Creating cycles: ", cyclesAmount);
		for (int t = 0; t < cyclesAmount; t++) {
			Cycle cycle = Cycle(t);
			cycle.round = t;
			//move saved agents to next cycle
			cycle.agents = tempAgents;
			RunCycle(&cycle);
			//move agents from previous cycle to temp
			tempAgents = cycle.agents;
			cycles.push_back(cycle);
		}

		logger->AddLog("Monte carlo simulation is done");
	}

	
}