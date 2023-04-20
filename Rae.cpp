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

	void Agent::CopyValues(Agent agent)
	{
		this->isStrategicAgent = agent.isStrategicAgent;
		this->number = agent.number;
		this->wasRecipient = agent.wasRecipient;
		this->serviceAvailiability = agent.serviceAvailiability;
		this->serviceReception = agent.serviceReception;
		this->suppliersNumbers = agent.suppliersNumbers;
		this->suppliersAmount = agent.suppliersAmount;
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
		bool shouldSetSuppilerForCurrentRecipient = mCurrentRecipient.suppliersAmount == 0;
		if (shouldSetSuppilerForCurrentRecipient) {
			SetSuppliersAmountForRecipient();
			return;
		}

		//here goes the interaction
		bool shouldInteract = mSuppliers.size() < mCurrentRecipient.suppliersAmount;
		if (shouldInteract) {
			Interact();
			return;
		}

		//do reporting switch to next one
		bool shouldMoveToNextRecipient = mCurrentRecipient.suppliersAmount == mInteractionIndex && mCurrentRecipient.number < mAgents.size()-1;
		if (shouldMoveToNextRecipient) {
			
			//preserve values of recipient
			mAgents[mCurrentRecipient.number].CopyValues(mCurrentRecipient);

			//TODO do report

			//move to next one
			int indexOfRecipient = mCurrentRecipient.number + 1;
			mCurrentRecipient = mAgents[indexOfRecipient];
		
			return;
		}

		//here the current cycle is done let's check if it's last one
		bool shouldSwitchToNextCycle = mCurrentCycle.round < cyclesAmount - 1;
		if (shouldSwitchToNextCycle) {
			MoveToNextCycle();
			return;
		}

		//here we are done
		mIsRunning = false;
		logger->AddLog("Monte Carlo simulation is done");
	}

	void MonteCarlo::Interact()
	{
		if (boostMode) {
			for (; mInteractionIndex < mCurrentRecipient.suppliersNumbers.size();) {
				MoveToNextInteraction();
				LogInteraction();
			}

			return;
		}

		LogInteraction();
		MoveToNextInteraction();
	}

	void MonteCarlo::LogInteraction() {
		std::string interactMessage = " Interaction index: ";
		interactMessage
			.append(std::to_string(mInteractionIndex))
			.append(" ;Cycle: ")
			.append(std::to_string(mCurrentCycle.round))
			.append(" ;Recipent: ")
			.append(std::to_string(mCurrentRecipient.number))
			.append(" is interacting with Suppiler agent number: ")
			.append(std::to_string(mCurrentSupplier.number));

		logger->AddLog(interactMessage.c_str());

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

	int MonteCarlo::SetSuppliersAmountForRecipient()
	{
		//reset suppilers
		mSuppliers.clear();
		mInteractionIndex = 0;

		Randomizer randomizer = Randomizer(kMin, kMax);
		mCurrentRecipient.suppliersAmount = randomizer.GetEvenRandomNumber();
		mCurrentRecipient.wasRecipient = true;

		//preserve
		mAgents[mCurrentRecipient.number].CopyValues(mCurrentRecipient);

		//set suppilers
		int suppilersIndexTopBoundary = mCurrentRecipient.suppliersAmount-1;
		Randomizer suppilerRandomizer = Randomizer(0, suppilersIndexTopBoundary);
		mCurrentRecipient.suppliersNumbers = suppilerRandomizer.GetEvenDistribute(mCurrentRecipient.suppliersAmount);

		for (int& suppilerIndex : mCurrentRecipient.suppliersNumbers) {
			//special case handling
			while (mCurrentRecipient.number == suppilerIndex) {
				suppilerIndex = suppilerRandomizer.GetEvenRandomNumber();
			}
		}

		std::string message = "";
		message.append("Choosen the suppliers amount for recipient number : ");
		message.append(std::to_string(mCurrentRecipient.number));
		message.append(" with ");
		message.append(std::to_string(mCurrentRecipient.suppliersAmount));
		message.append(" suppilers.").append("Cycle: ").append(std::to_string(mCurrentCycle.round));

		logger->AddLog(message.c_str());

		//set first suppiler for agent
		mInteractionIndex = 0;
		int suppilerNumber = mCurrentRecipient.suppliersNumbers[mInteractionIndex];
		mCurrentSupplier = mAgents[suppilerNumber];
		
		return mCurrentRecipient.suppliersAmount;
	}

	void MonteCarlo::MoveToNextInteraction() {
		int suppilerNumber = mCurrentRecipient.suppliersNumbers[mInteractionIndex];

		//preserve supplier
		mAgents[mCurrentSupplier.number].CopyValues(mCurrentSupplier);

		mCurrentSupplier = mAgents[suppilerNumber];

		mSuppliers.push_back(mCurrentSupplier);
		mInteractionIndex++;

	}

	void MonteCarlo::MoveToNextCycle()
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

	
}

