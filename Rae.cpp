#include "Rae.h"
#include <vector>
#include <random>
#include <thread>
#include <chrono>

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

	
	void MonteCarlo::CreateAgents()
	{
		std::vector<Agent> agents;

		for (int i = 0; i < agentsAmount; i++) {
			Agent agent = Agent();
			agent.number = i;
			agent.trust = beginTrustMesaure;

			if (i < strategicAgentsAmount) {
				agent.isStrategicAgent = true;
			}

			mAgents.push_back(agent);
		}

		std::string agentsCreated = "Created agents: ";
		agentsCreated.append(std::to_string(agentsAmount));

		agentsCreated.append(". Strategic agents: ");
		agentsCreated.append(std::to_string(strategicAgentsAmount));
		agentsCreated.append(". Honest agents: ");
		agentsCreated.append(std::to_string(agentsAmount - strategicAgentsAmount));
		agentsCreated.append(".");

		logger->AddLog(agentsCreated.c_str());
	}

	void MonteCarlo::CreateCycles()
	{
		std::vector<Cycle> cycles;

		logger->AddLog("Creating cycles: ", cyclesAmount);
		for (int t = 0; t < cyclesAmount; t++) {
			Cycle cycle = Cycle(t);
			cycle.round = t;
			cycle.SetAgents(mAgents);
			mCycles.push_back(cycle);
		}
	}

	void MonteCarlo::SetServiceAvailiabilityForAgent(Agent* agent)
	{
		logger->AddLog("setting service availiability for agent: ", agent->number);
		RealRandomizer agentServiceRandomizer = RealRandomizer(0, 1);

		double serviceA = 0;
		double availiability = 0;

		do
		{
			serviceA = agentServiceRandomizer.GetEvenRandomNumber();
			availiability = std::pow(serviceA, expoA);
			agent->serviceAvailiability = availiability;
			logger->AddLog("setting avialibility: ", availiability);
		} while (availiability >= serviceA);
	}

	void MonteCarlo::SetServiceReceptionForAgent(Agent* agent)
	{
		logger->AddLog("setting reception for agent", agent->number);
		RealRandomizer agentServiceRandomizer = RealRandomizer(0, 1);

		double reception = 0;
		double serviceA = 0;

		do {
			serviceA = agentServiceRandomizer.GetEvenRandomNumber();
			reception = std::pow(serviceA, expoG);
			agent->serviceReception = reception;
			logger->AddLog("Reception value set: ", reception);
		} while (reception >= serviceA);
	}

	void MonteCarlo::SwitchToNextCycle()
	{
		//preserve agents as copy
		mAgents = mCurrentCycle->GetAgents();

		//get next cycle 
		int next = mCurrentCycle->round+1;
		Cycle *nextCycle = &mCycles[next];

		//set agents for next cycle
		nextCycle->SetAgents(mAgents);
		mCurrentCycle = nextCycle;

		nextCycle->Start();
	}

	void MonteCarlo::Start()
	{
		logger->AddLog("Starting initialization");

		CreateAgents();
		CreateCycles();

		//first one
		MonteCarlo::mCurrentCycle = &mCycles[0];

		logger->AddLog("Initialization is done!");
		logger->AddLog("Starting Monte Carlo simulation");

		mCurrentCycle->Start();

		MonteCarlo::mIsRunning = true;
	}

	void MonteCarlo::Update()
	{
		if (mIsRunning == false) return;
		if (mCurrentCycle->IsDone()) {
			logger->AddLog("Switching to next cycle", mCurrentCycle->round);
			SwitchToNextCycle();
		}
		
		
	}

	bool Cycle::IsDone()
	{
		int currentAgentCount =	mRecipientAgent->number + 1;
		if (currentAgentCount == mAgents.size()) 
			return true;
		
		return false;
	}



	void Cycle::Start()
	{
		mRecipientAgent = &mAgents[0]; //first;
	}

	//void MonteCarlo::ChooseSuppilers(Cycle* cycle)
	//{
	//	for (Agent& recipent : cycle->agents) {
	//		std::this_thread::sleep_for(std::chrono::seconds(2));
	//		//recipent would be the one from round
	//		recipent.wasRecipent = true;

	//		logger->AddLog("Choosing the suppliers agents for agent: ", recipent.number);
	//		Randomizer randomizer = Randomizer(kMin, kMax);
	//		int suppilersAmount = randomizer.GetEvenRandomNumber();

	//		int amountOfAgentsLeft = agentsAmount - 1;
	//		Randomizer supplierRandomizer = Randomizer(0, amountOfAgentsLeft); //amount of agents to choose from

	//		std::vector<int> cycleAgentSuppilerNumbers = supplierRandomizer.GetEvenDistribute(suppilersAmount); //distribute random from amount of agents to choose

	//		std::vector<Agent> suppilers;
	//		int choosenRecipentsCounter = 0;
	//		for (int& number : cycleAgentSuppilerNumbers) {
	//			//special case handle when we got recipent inside suppiler / shouldn't happen so often
	//			while (number == recipent.number) {
	//				number = supplierRandomizer.GetEvenRandomNumber();
	//			}

	//			Agent supplier = cycle->agents[number];
	//			//SetServiceAvailiabilityForAgent(&supplier);
	//			suppilers.push_back(supplier);
	//		}
	//

	//		logger->AddLog("Amount of suppilers choosen: ", suppilersAmount);
	//	}
	//}
}