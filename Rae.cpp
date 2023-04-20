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

	Agent AgentsFactory::Create()
	{
		Agent agent = Agent();
		agent.trust = mBeginingTrustLevel;
		return agent;
	}

	Agent AgentsFactory::Create(int number)
	{
		Agent agent = Create();
		agent.number = number;

		if (number < mStrategicCount) {
			agent.isStrategicAgent = true;
		}

		mLogger->AddLog("Created agent:", agent.number);

		return agent;
	}

	Cycle CycleFactory::Create(int number)
	{
		Cycle cycle = Cycle(number);
		mLogger->AddLog("Created cycle: ", cycle.round);
		return cycle;
	}

	void MonteCarlo::Start()
	{
		logger->AddLog("Starting initialization of Monte Carlo Simulation");

		mIsInitalizing = false;

		//reset
		mAgents.clear();
		mCycles.clear();

		mAgentsFactory = AgentsFactory(beginTrustMesaure, strategicAgentsAmount, logger);
		mCycleFactory = CycleFactory(logger);

		MonteCarlo::mIsInitalizing = true;
	}

	void MonteCarlo::Update()
	{
		if (mIsInitalizing) {
			Initialize();
			return;
		}

		if (mIsRunning) {
			Run();
		}
	}

	void MonteCarlo::Run()
	{
		//here procceding with agents for current cycle recipient
		bool shouldSetSuppilerForCurrentRecipient = mCurrentRecipient.suppilersAmount == 0;
		if (shouldSetSuppilerForCurrentRecipient) {
			SetSuppilersAmountForRecipient();
			return;
		}

		//here goes the interaction
		bool shouldInteract = mSuppilers.size() < mCurrentRecipient.suppilersAmount;
		if (shouldInteract) {
			std::string interactMessage =" Interaction index: ";

			interactMessage
				.append(std::to_string(mInteractionIndex))
				.append(" ;Cycle: ")
				.append(std::to_string(mCurrentCycle.round))
				.append(" ;Recipent: ")
				.append(std::to_string(mCurrentRecipient.number))
				.append(" is interacting with Suppiler agent number: ")
				.append(std::to_string(mCurrentSuppiler.number));

			logger->AddLog(interactMessage.c_str());

			MoveToNextInteraction();

			return;
		}


		//here the current cycle is done let's check if it's last one
		bool shouldSwitchToNextCycle = mCurrentCycle.round < cyclesAmount - 1;
		if (shouldSwitchToNextCycle) {
			SwitchToNextCycle();
			return;
		}

		//here we are done
		mIsRunning = false;
		logger->AddLog("Monte Carlo simulation is done");
	}

	void MonteCarlo::Initialize()
	{
		//create agents
		int agentsCount = (int)mAgents.size();
		bool shouldCreateAgents = agentsCount < agentsAmount;
		if (shouldCreateAgents) {
			Agent agent = mAgentsFactory.Create(mAgents.size());
			mAgents.push_back(agent);
			return;
		}

		//create cycles
		bool shouldCreateCycles = mCycles.size() < cyclesAmount;
		if (shouldCreateCycles)
		{
			Cycle cycle = mCycleFactory.Create((int)mCycles.size());
			mCycles.push_back(cycle);
			return;
		}

		//start with first agent and cycle
		mCurrentCycle = mCycles[0];
		mCurrentRecipient = mAgents[0];

		//got to the end, all agents and cycles are initialized
		mIsInitalizing = false;
		logger->AddLog("Initialization is done.");
		logger->AddLog("Running with cycle: ", mCurrentCycle.round);

		mIsRunning = true;
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
		
		serviceA = agentServiceRandomizer.GetEvenRandomNumber();
		reception = std::pow(serviceA, expoG);
		agent->serviceReception = reception;
		logger->AddLog("Reception value set: ", reception);
	}

	int MonteCarlo::SetSuppilersAmountForRecipient()
	{
		//reset suppilers
		mSuppilers.clear();
		mInteractionIndex = 0;

		Randomizer randomizer = Randomizer(kMin, kMax);
		mCurrentRecipient.suppilersAmount = randomizer.GetEvenRandomNumber();
		mCurrentRecipient.wasRecipient = true;

		//preserve
		mAgents[mCurrentRecipient.number].CopyValues(mCurrentRecipient);

		//set suppilers
		int suppilersIndexTopBoundary = mCurrentRecipient.suppilersAmount-1;
		Randomizer suppilerRandomizer = Randomizer(0, suppilersIndexTopBoundary);
		mCurrentRecipient.suppilersNumbers = suppilerRandomizer.GetEvenDistribute(mCurrentRecipient.suppilersAmount);

		for (int& suppilerIndex : mCurrentRecipient.suppilersNumbers) {
			//special case handling
			while (mCurrentRecipient.number == suppilerIndex) {
				suppilerIndex = suppilerRandomizer.GetEvenRandomNumber();
			}
		}

		std::string message = "";
		message.append("Choosen the suppliers amount for recipient number : ");
		message.append(std::to_string(mCurrentRecipient.number));
		message.append(" with ");
		message.append(std::to_string(mCurrentRecipient.suppilersAmount));
		message.append(" suppilers");

		logger->AddLog(message.c_str());

		//set first suppiler for agent
		mInteractionIndex = 0;
		int suppilerNumber = mCurrentRecipient.suppilersNumbers[mInteractionIndex];
		mCurrentSuppiler = mAgents[suppilerNumber];
		
		return mCurrentRecipient.suppilersAmount;
	}

	void MonteCarlo::MoveToNextInteraction() {
		int suppilerNumber = mCurrentRecipient.suppilersNumbers[mInteractionIndex];
		mCurrentSuppiler = mAgents[suppilerNumber];
		mSuppilers.push_back(mCurrentSuppiler);
		mInteractionIndex++;

	}

	void MonteCarlo::SwitchToNextCycle()
	{
		logger->AddLog("Current cycle done: ", mCurrentCycle.round);
		//preserve agents in done as copy
		mCycles[mCurrentCycle.round].SetAgents(mAgents);

		//get next cycle
		int next = mCurrentCycle.round + 1;
		Cycle nextCycle = mCycles[next];

		//set next cycle
		mCurrentCycle = nextCycle;
		logger->AddLog("Moved to next cycle: ", mCurrentCycle.round);
		//reset agents
		logger->AddLog("Reseting agents");
		for (Agent& agent : mAgents) {
			agent.ResetForNextCycle();
		}

		mCurrentRecipient = mAgents[0];
	}

	MonteCarlo::MonteCarlo(RaeLogger* logger)
	{
		this->logger = logger;
	}

	void Agent::CopyValues(Agent agent)
	{
		this->isStrategicAgent = agent.isStrategicAgent;
		this->number = agent.number;
		this->serviceAvailiability = agent.serviceAvailiability;
		this->serviceReception = agent.serviceReception;
		this->suppilersNumbers = agent.suppilersNumbers;
		this->suppilersAmount = agent.suppilersAmount;
	}

	
}

