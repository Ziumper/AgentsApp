#pragma once
#include <vector>
#include <memory>
#include <string_view>
#include <random>
#include <string>
#include <map>

namespace Rae
{
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

	class GoodWill {
	public:
		double x{ 0 };
		double y{ 0 };
		double z{ 0 };
		GoodWill() {};
	};

	class RaeLogger {
	public:
		virtual void AddLog(const char* entry) = 0;
		virtual void AddLog(const char* entry, const int Number) = 0;
		virtual void AddLog(const char* entry, const double Number) = 0;
		virtual ~RaeLogger() {};
	};

	class Agent {
	public:
		int Number{ 0 };
		double trust{ 0 };
		double serviceAvailiability{ 0 };
		double serviceReception{ 0 };
		bool isStrategicAgent{ false };
		bool wasRecipient{ false };
		int suppliersAmount{ 0 };
		std::vector<int> suppliersNumbers;
		void ResetForNextCycle() {
			suppliersAmount = 0; 
			serviceReception = 0;
			serviceAvailiability = 0;
		}

		void CopyValues(Agent agent);
		double CalculateRaeIT();
	};

	class Cycle {
		std::vector<Agent> mAgents;
	public:
		int Round{ 0 };
		Cycle(int roundNumber) : Round(roundNumber) {};
		Cycle() : Round(0) {};
		void SetAgents(std::vector<Agent> agents) { mAgents = agents; };
		std::vector<Agent> GetAgents() { return mAgents; }
	};



	class AgentsFactory {
	private:
		double mBeginingTrustLevel{ 0 };
		int mStrategicCount{ 0 };
		RaeLogger* mLogger{ 0 };
	public:
		AgentsFactory() {};
		AgentsFactory(double trustLevel,int strategic, RaeLogger *logger) :
			mBeginingTrustLevel(trustLevel),
			mStrategicCount(strategic),
			mLogger(logger)
		{};
		Agent Create();
		Agent Create(int Number);
	};

	class CycleFactory {
	private:
		RaeLogger* mLogger{ 0 };
	public:
		CycleFactory() {};
		CycleFactory(RaeLogger *logger): mLogger(logger) {};
		Cycle Create(int Number);
	};

	class ReportedService {
	public:
		int RecipientNumber;
		double reportedValue;
	};

	class MonteCarlo {
	private:
		Cycle mCurrentCycle;
		Agent mCurrentRecipient;
		Agent mCurrentSupplier;
		int mInteractionIndex{ 0 };
		std::vector<Agent> mAgents;
		std::vector<Cycle> mCycles;
		std::vector<Agent> mSuppliers;

		/// <summary>
		///Sum of each Agent reported 
		/// </summary>
		std::map<int,double> mReportedSumForInteraction;
		/// <summary>
		/// Average Ri(t) for agent i
		/// </summary>
		std::map<int, double> mReportedAverage;
		bool mIsRunning{ false };
		bool mIsInitalizing{ false };
		void SetServiceAvailiabilityForSupplier();
		void SetServiceReceptionForRecipient();
		void Interact();
		void Run();
		void Initialize();
		void LogInteraction();
		void MoveToNextInteraction();
		void MoveToNextCycle();
		void MoveToNextRecipient();
		bool UpdateAgents();
		int SetSuppliersAmountForRecipient();
		void CreateAgent();
		void UpdateInteraction();
		void ReportAgents();
		AgentsFactory mAgentsFactory;
		CycleFactory mCycleFactory;
	public:
		int cyclesAmount{ 3 };
		int agentsAmount{ 1000 };
		int strategicAgentsAmount{ 50 };
		int kMin{ 50 };
		int kMax{ 150 };
		double expoA{ 0.5 };
		double expoG{ 0.5 };
		double beginTrustMesaure{ 1 };
		bool boostMode{ false };
		RaeLogger* logger;
		GoodWill goodWill{ GoodWill() };
		MonteCarlo(RaeLogger* logger);
		void Start();
		void Update();
		int CurrentRecipientNumberInCycle() { return mCurrentRecipient.Number+1; }
		int CurrentCycleNumber() { return mCurrentCycle.Round; }
		bool IsWorking(){ return mIsRunning || mIsInitalizing; }
		std::vector<float> GetTrustLevels();
			
		
	
		//Agent GetSuppilerForRecipient();
	};

	//TODO
	/*
	* 1. Przerobiæ konstruktory
	* 2. Przekopiowaæ referencje do tablicy z mCurrentCycle do mCycles
	* 3. To samo zrobiæ dla agentów - punkt 2
	* 4. Update dla agenta i losowanie pojedyñczego agenta
	*/
}