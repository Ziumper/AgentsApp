#include "Rae.h"
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <algorithm>
#include "Randomizer.h"
#include "KMeans.h"


namespace Rae {

	void Agent::CopyValues(Agent agent)
	{
		this->isStrategicAgent = agent.isStrategicAgent;
		this->Number = agent.Number;
		this->wasRecipient = agent.wasRecipient;
		this->serviceAvailiability = agent.serviceAvailiability;
		this->serviceReception = agent.serviceReception;
		this->suppliersNumbers = agent.suppliersNumbers;
		this->suppliersAmount = agent.suppliersAmount;
		this->HonestSuppilersCount = agent.HonestSuppilersCount;
		this->StrategicSuppliersCount = agent.StrategicSuppliersCount;
		this->AviliabilitySupplierSum = agent.AviliabilitySupplierSum;
	}

	double Agent::CalculateRaeIT()
	{
		return trust * RecipientPolicy;
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
		mGeneratingReportDone = false;

		mAgentsFactory = AgentsFactory(beginTrustMesaure, strategicAgentsAmount, logger);
		mCycleFactory = CycleFactory(logger);

		MonteCarlo::mIsInitalizing = true;
		Done = false;
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

	void MonteCarlo::ApplyNewTrustLevels() {
		//clusterization
		logger->AddLog("Starting clusterization");
		KMeans kMeans = KMeans(2, this->mReportedAverage);
		kMeans.ProcessKMeansClusterization();

		std::vector<Centroid> centroids = kMeans.GetCentroids();

		Centroid high;
		Centroid low;

		if (centroids[0].Value > centroids[1].Value) {
			high = centroids[0];
			low = centroids[1];
		}
		else {
			high = centroids[1];
			low = centroids[0];
		}

		//apply trust levels V(i +1) 
		logger->AddLog("Appling trust levels from RAE to agents for next cycle");
		//sum 1

		//Noirmalization process
		//average low caluclations
		double sum = 0;
		int lowCount = (int)low.Assigned.size();
		for (int& assigned : low.Assigned) {
			double assignedReportedAverage = mReportedAverage[assigned];
			sum = sum + assignedReportedAverage;
		}

		double averageLow = sum / lowCount;

		//average high calculations
		sum = 0;
		int highCount = (int)high.Assigned.size();
		for (int& assigned : high.Assigned) {
			double assignedReportedAverage = mReportedAverage[assigned];
			sum = sum + assignedReportedAverage;
		}

		double averageHigh = sum / lowCount;

		//nromalization for each of agents gets always the average value of reported average 
		for (Agent& agent : mAgents) {
			//find if it's in low or high
			bool isHigh = false;

			isHigh = high.IsAssigned(agent.Number);

			if (isHigh) {
				agent.trust = averageLow / averageHigh;
				continue;
			}

			agent.trust = averageLow;
		}

		this->mCycleTempStategicTraectory = 0;
		this->mCycleTempHonestTraectory = 0;

		double strategicTrustSum = 0;
		double honestTrustSum = 0;

		logger->AddLog("Calculationg trajectory for cycles");
		for (Agent& agent : mAgents) {
			if (agent.isStrategicAgent) {
				strategicTrustSum = strategicTrustSum + agent.trust;
				continue;
			}
			
			//normal honest one
			honestTrustSum = honestTrustSum + agent.trust;
		}

		int honestAgentsAmount = agentsAmount - strategicAgentsAmount;
		this->mCycleTempHonestTraectory = honestTrustSum / honestAgentsAmount;
		this->mCycleTempStategicTraectory = strategicTrustSum / strategicAgentsAmount;

		logger->AddLog("Calculating netto outflow");
		this->mCycleTempOutflow = 0;
		double honestNettoSum = 0; //i in Honest Sum of suppliers availiability 
		double strategicNettoSum = 0; //i in Strategic Sum of suppliers availiability 
		int strategicCountNetto = 0; //amount of suppilers with honest 
		int honestCountNetto = 0;//amount of suppliers with stategic 
		
		for (Agent& agent : mAgents) {
			if (agent.isStrategicAgent) {
				strategicNettoSum = strategicNettoSum + agent.AviliabilitySupplierSum;
				strategicCountNetto = strategicCountNetto + agent.HonestSuppilersCount;
				continue;
			}

			//normal honest one
			honestNettoSum = honestNettoSum + agent.AviliabilitySupplierSum;
			honestCountNetto = honestCountNetto + agent.StrategicSuppliersCount;
		}

		double calculationLeft = honestNettoSum / honestCountNetto;
		double calculationRight = strategicNettoSum / strategicCountNetto;

		this->mCycleTempOutflow = calculationLeft - calculationRight;
	}


	
	void MonteCarlo::UpdateInteraction()
	{
		SetServiceReceptionForRecipient();
		SetServiceAvailiabilityForSupplier();


		double tresholdrij = CalculateRecipientTresholdValue();
		double tresholdpij = CalculateSupplierTresholdValue();

		//calculating Pij - suppiler policy
		mCurrentSupplier.SupplierPolicy = std::min(mCurrentSupplier.serviceAvailiability, tresholdpij);
		
		//calculating Rij - recipient policy
		mCurrentRecipient.RecipientPolicy = std::min(mCurrentRecipient.serviceReception, tresholdrij);


		//calculation for netto outflow
		mCurrentRecipient.AviliabilitySupplierSum = mCurrentRecipient.AviliabilitySupplierSum + mCurrentSupplier.SupplierPolicy;

		if (mCurrentSupplier.isStrategicAgent)
			mCurrentRecipient.StrategicSuppliersCount = mCurrentRecipient.StrategicSuppliersCount + 1;
		else mCurrentRecipient.HonestSuppilersCount = mCurrentRecipient.HonestSuppilersCount + 1;
			
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
		ApplyNewTrustLevels();

		//here the current cycle is done let's check if it's not last one
		bool shouldSwitchToNextCycle = mCurrentCycle.Round < cyclesAmount - 1;
		if (shouldSwitchToNextCycle) {
			MoveToNextCycle();
			return;
		}

		//here we are done so
		//preserve last cycle for reporting and reset cycle current number
		//preserve
		bool isLast = this->mCurrentCycle.Round == this->mCycles.size()-1;
		if (isLast) {
			PreserveCycle();
			//reset current cycle round
		}
	
		mIsRunning = false;
		logger->AddLog("Monte Carlo simulation is done");
		if (mGeneratingReportDone == false) { 
			GenerateReport(); 
			Done = true;
			return;
		}

		
	}

	void MonteCarlo::PreserveCycle() {
		mCycles[this->mCurrentCycle.Round].SetAgents(mAgents);
		mCycles[this->mCurrentCycle.Round].HonestTraectory = this->mCycleTempHonestTraectory;
		mCycles[this->mCurrentCycle.Round].NetOutflow = this->mCycleTempOutflow;
		mCycles[this->mCurrentCycle.Round].StrategicTraectory = this->mCycleTempStategicTraectory;
	}

	//Calculating rij
	double MonteCarlo::CalculateRecipientTresholdValue()
	{
		//L(Vi(t),x) - case i is S , and J is H
		if (mCurrentSupplier.isStrategicAgent && mCurrentRecipient.isStrategicAgent == false) {
			double honest = CalculateHonestRecipient(mCurrentRecipient.serviceReception,
				mCurrentSupplier.serviceReception,
				mCurrentRecipient.trust,
				goodWill.x);
			return honest;
		}

		//min{z,LVi(t),x) - case i is H and J is S
		if (mCurrentSupplier.isStrategicAgent == false && mCurrentRecipient.isStrategicAgent) {
			double honestPolicy = CalculateHonestRecipient(mCurrentRecipient.serviceReception,
				mCurrentSupplier.serviceAvailiability,
				mCurrentRecipient.trust,
				goodWill.x);
			double min = std::min(goodWill.z, honestPolicy);
			min = std::min(goodWill.x, min);
			return min;
		}

		//case i is S and , J is S return 1
		return 1;
	}

	//Calculating pij
	double MonteCarlo::CalculateSupplierTresholdValue()
	{
		//min{y,LVj(t),x)} j is H and i is S
		if (mCurrentRecipient.isStrategicAgent == false && mCurrentSupplier.isStrategicAgent) {
			double honestPolicy = CalculateHonestSupplier(mCurrentSupplier.serviceAvailiability,
				mCurrentSupplier.trust,
				goodWill.x);
			double min = std::min(goodWill.y, honestPolicy);
			min = std::min(goodWill.x, min);
			return min;
		}

		//L(Vj(t),x) j is Strategic Agent and i is honest
		if (mCurrentRecipient.isStrategicAgent && mCurrentSupplier.isStrategicAgent == false) {
			double honestPolicy = CalculateHonestSupplier(mCurrentSupplier.serviceAvailiability,
				mCurrentSupplier.trust,
				goodWill.x);
			return honestPolicy;
		}

		//case i is S and , J is S return 1
		return 1;
	}

	//TODO add arguments to functions Pij
	double MonteCarlo::CalculateHonestSupplier(double aij, double trustLevel, double goodWill)
	{
		double subtract = 1 - goodWill;
		if (trustLevel >= subtract) return aij;

		return 0;
	}

	//TODO add arguments to functions Rij
	double MonteCarlo::CalculateHonestRecipient(double gij, double pij,double trustLevel, float goodWill)
	{
		double subtract = 1 - goodWill;
		if (trustLevel >= subtract) return gij*pij;

		return 0;
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

	//Aij(t)
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

	//Gij(t) 
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
		
		//start moving to next one
		logger->AddLog("Current cycle done: ", mCurrentCycle.Round);

		PreserveCycle();
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

		

		mReportedAverage.clear();
		mReportedSumForInteraction.clear();
		mCurrentRecipient = mAgents[0];
	}

	void MonteCarlo::MoveToNextRecipient() {
		//preserve first
		mAgents[mCurrentRecipient.Number].CopyValues(mCurrentRecipient);
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

		//preserve last one recipient values
		if (mCurrentRecipient.Number == mAgents.size() - 1) {
			mAgents[mCurrentRecipient.Number].CopyValues(mCurrentRecipient);
		}

		return false;
	}

	MonteCarlo::MonteCarlo(RaeLogger* logger)
	{
		this->logger = logger;
	}

	std::vector<float> MonteCarlo::GetTrustLevels() {
		std::vector<float> trust;
		
		for (Agent& agent : mAgents) {
			trust.push_back(agent.serviceReception);
		}

		return trust;
	}


	void MonteCarlo::GenerateReport() {
		logger->AddLog("Generating report data");

		SAgentTraectoryAvg.clear();
		HAgentTraectoryAvg.clear();
		NettoOutflow.clear();
		FinalTrust.clear();
		CycleNumbersForPlot.clear();

		for (Cycle& cycle : mCycles) {
			SAgentTraectoryAvg.push_back(cycle.StrategicTraectory);
			HAgentTraectoryAvg.push_back(cycle.HonestTraectory);
			NettoOutflow.push_back(cycle.NetOutflow);
			CycleNumbersForPlot.push_back((float)cycle.Round);
		}

		for (Agent& agent : mAgents) {
			FinalTrust.push_back(agent.trust);
		}

		mGeneratingReportDone = true;
		logger->AddLog("Generating report data is done");
	}
}

