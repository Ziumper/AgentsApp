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
		this->Number = agent.Number;
		this->wasRecipient = agent.wasRecipient;
		this->serviceAvailiability = agent.serviceAvailiability;
		this->serviceReception = agent.serviceReception;
		this->suppliersNumbers = agent.suppliersNumbers;
		this->suppliersAmount = agent.suppliersAmount;
	}

	double Agent::CalculateRaeIT()
	{
		return trust * serviceReception;
	}

	Agent AgentsFactory::Create()
	{
		Agent agent = Agent();
		agent.trust = mBeginingTrustLevel;
		return agent;
	}

	Agent AgentsFactory::Create(int Number)
	{
		Agent agent = Create();
		agent.Number = Number;

		if (Number < mStrategicCount) {
			agent.isStrategicAgent = true;
		}

		mLogger->AddLog("Created agent:", agent.Number);

		return agent;
	}

	Cycle CycleFactory::Create(int Number)
	{
		Cycle cycle = Cycle(Number);
		mLogger->AddLog("Created cycle: ", cycle.Round);
		return cycle;
	}

	void MonteCarlo::Start()
	{
		logger->AddLog("Starting initialization of Monte Carlo Simulation");

		mIsInitalizing = false;

		//reset
		mAgents.clear();
		mCycles.clear();
		mReportedAverage.clear();
		mReportedSumForInteraction.clear();

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

	void MonteCarlo::ReportAgents()
	{
		for (Agent& agent : mAgents) {
			double average = mReportedSumForInteraction[agent.Number]/ agent.suppliersAmount;
			std::string message = "Counted average Ri(t) for agent i: ";
			message.append(std::to_string(agent.Number))
				.append(". Cycle: ")
				.append(std::to_string(mCurrentCycle.Round))
				.append(". Average: ").append(std::to_string(average));

			mReportedAverage[agent.Number] = average;

			logger->AddLog(message.c_str());
		}
	}

	void MonteCarlo::UpdateInteraction()
	{
		SetServiceReceptionForRecipient();
		SetServiceAvailiabilityForSupplier();

		//TODO calculate some policy. // it's random so far
		RealRandomizer randomizer = RealRandomizer(0, 1);

		/// <summary>
		/// Rij
		/// </summary>
		double recipientPolicy = randomizer.GetEvenRandomNumber();

		/// <summary>
		/// Pij
		/// </summary> 
		double suppilerPolicy = randomizer.GetEvenRandomNumber();

		mReportedSumForInteraction[mCurrentRecipient.Number] += mCurrentRecipient.CalculateRaeIT();

		LogInteraction();
		MoveToNextInteraction();
	}

	void MonteCarlo::CreateAgent()
	{
		Agent agent = mAgentsFactory.Create(mAgents.size());
		mReportedSumForInteraction[agent.Number] = 0;
		mAgents.push_back(agent);
	}

	void MonteCarlo::Run()
	{
		bool shouldUpdateAgent = UpdateAgents();
		if (shouldUpdateAgent) return;

		ReportAgents();
		
		//here the current cycle is done let's check if it's last one
		bool shouldSwitchToNextCycle = mCurrentCycle.Round < cyclesAmount - 1;
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
				UpdateInteraction();
			}

			return;
		}

		UpdateInteraction();
	}

	void MonteCarlo::LogInteraction() {
		std::string interactMessage = " Interaction index: ";
		interactMessage
			.append(std::to_string(mInteractionIndex))
			.append(" ;Cycle: ")
			.append(std::to_string(mCurrentCycle.Round))
			.append(" ;recipient: ")
			.append(std::to_string(mCurrentRecipient.Number))
			.append(" is interacting with Suppiler agent number: ")
			.append(std::to_string(mCurrentSupplier.Number));

		logger->AddLog(interactMessage.c_str());

	}

	void MonteCarlo::Initialize()
	{
		//create agents
		int agentsCount = (int)mAgents.size();
		bool shouldCreateAgents = agentsCount < agentsAmount;
		if (shouldCreateAgents) {
			if (boostMode) {
				for (; mAgents.size() < agentsAmount;) {
					CreateAgent();
				}

				return;
			}

			CreateAgent();
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
		logger->AddLog("Running with cycle: ", mCurrentCycle.Round);

		mIsRunning = true;
	}

	void MonteCarlo::SetServiceAvailiabilityForSupplier()
	{
		RealRandomizer agentServiceRandomizer = RealRandomizer(0, 1);

		double serviceA = 0;
		double availiability = 0;
		serviceA = agentServiceRandomizer.GetEvenRandomNumber();
		availiability = std::pow(serviceA, expoA);
		mCurrentSupplier.serviceAvailiability = availiability;

		std::string message = "setting service availiability for agent supplier: ";
		message.append(std::to_string(mCurrentSupplier.Number))
			.append(". Service avialiability: ")
			.append(std::to_string(mCurrentSupplier.serviceAvailiability));
		logger->AddLog(message.c_str());
	}

	void MonteCarlo::SetServiceReceptionForRecipient()
	{
		RealRandomizer agentServiceRandomizer = RealRandomizer(0, 1);

		double reception = 0;
		double serviceA = 0;
		
		serviceA = agentServiceRandomizer.GetEvenRandomNumber();
		reception = std::pow(serviceA, expoG);
		mCurrentRecipient.serviceReception = reception;

		std::string message = "setting service recipient for agent supplier: ";
		message.append(std::to_string(mCurrentRecipient.Number))
			.append(". Service reception: ")
			.append(std::to_string(mCurrentRecipient.serviceReception));
		logger->AddLog(message.c_str());
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
		mAgents[mCurrentRecipient.Number].CopyValues(mCurrentRecipient);

		//set suppilers
		int suppilersIndexTopBoundary = mCurrentRecipient.suppliersAmount-1;
		Randomizer suppilerRandomizer = Randomizer(0, suppilersIndexTopBoundary);
		mCurrentRecipient.suppliersNumbers = suppilerRandomizer.GetEvenDistribute(mCurrentRecipient.suppliersAmount);

		for (int& suppilerIndex : mCurrentRecipient.suppliersNumbers) {
			//special case handling
			while (mCurrentRecipient.Number == suppilerIndex) {
				suppilerIndex = suppilerRandomizer.GetEvenRandomNumber();
			}
		}

		std::string message = "";
		message.append("Choosen the suppliers amount for recipient number : ");
		message.append(std::to_string(mCurrentRecipient.Number));
		message.append(" with ");
		message.append(std::to_string(mCurrentRecipient.suppliersAmount));
		message.append(" suppilers.").append("Cycle: ").append(std::to_string(mCurrentCycle.Round));

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
		mAgents[mCurrentSupplier.Number].CopyValues(mCurrentSupplier);

		mCurrentSupplier = mAgents[suppilerNumber];

		mSuppliers.push_back(mCurrentSupplier);
		mInteractionIndex++;

	}

	void MonteCarlo::MoveToNextCycle()
	{
		//TODO do reporting
		logger->AddLog("Starting reporting to RAE");

		//move to next one
		logger->AddLog("Current cycle done: ", mCurrentCycle.Round);
		//preserve agents in done as copy
		mCycles[mCurrentCycle.Round].SetAgents(mAgents);

		//get next cycle
		int next = mCurrentCycle.Round + 1;
		Cycle nextCycle = mCycles[next];

		//set next cycle
		mCurrentCycle = nextCycle;
		logger->AddLog("Moved to next cycle: ", mCurrentCycle.Round);
		//reset agents
		logger->AddLog("Reseting agents");
		for (Agent& agent : mAgents) {
			agent.ResetForNextCycle();
		}

		mCurrentRecipient = mAgents[0];
	}

	void MonteCarlo::MoveToNextRecipient() {
		//preserve values of recipient
		mAgents[mCurrentRecipient.Number].CopyValues(mCurrentRecipient);

		//we have finished the interactions so natural is to save report results to RAE and adjust trust values
	
		int indexOfRecipient = mCurrentRecipient.Number + 1;
		mCurrentRecipient = mAgents[indexOfRecipient];

	}

	/// <summary>
	/// Returns true whenever there is a need of update agent and does it as well
	/// </summary>
	/// <returns>bool value</returns>
	bool MonteCarlo::UpdateAgents()
	{
		//here procceding with agents for current cycle recipient
		bool shouldSetSuppilerForCurrentRecipient = mCurrentRecipient.suppliersAmount == 0;
		if (shouldSetSuppilerForCurrentRecipient) {
			SetSuppliersAmountForRecipient();
			return true;
		}

		//here goes the interaction
		bool shouldInteract = mSuppliers.size() < mCurrentRecipient.suppliersAmount;
		if (shouldInteract) {
			Interact();
			return true;
		}

		//move
		bool shouldMoveToNextRecipient = mCurrentRecipient.suppliersAmount == mInteractionIndex && mCurrentRecipient.Number < mAgents.size()-1;
		if (shouldMoveToNextRecipient) {
			MoveToNextRecipient();
			return true;
		}

		return false;
	}

	MonteCarlo::MonteCarlo(RaeLogger* logger)
	{
		this->logger = logger;
	}


}

