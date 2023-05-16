#pragma once
#include <vector>
#include <memory>
#include <string_view>
#include <random>
#include <string>
#include <map>
#include <unordered_map>

namespace Rae
{

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
		double SupplierPolicy{ 0 }; //P(i,j)
		double RecipientPolicy{ 0 }; //R(i,j)
		int Number{ 0 };
		double trust{ 0 };
		double serviceAvailiability{ 0 };
		double serviceReception{ 0 };
		bool isStrategicAgent{ false };
		bool wasRecipient{ false };
		int suppliersAmount{ 0 };
		int HonestSuppilersCount{ 0 };
		int StrategicSuppliersCount{ 0 };
		double AviliabilitySupplierSum{ 0 };
		std::vector<int> suppliersNumbers;
		void ResetForNextCycle() {
			suppliersAmount = 0; 
			serviceReception = 0;
			serviceAvailiability = 0;
			HonestSuppilersCount = 0;
			StrategicSuppliersCount = 0;
			AviliabilitySupplierSum = 0;
			SupplierPolicy = 0;
			RecipientPolicy = 0;
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
		double StrategicTraectory{ 0 };
		double HonestTraectory{ 0 };
		double NetOutflow{ 0 };
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
		Agent mCurrentRecipient; //this is j-agent
		Agent mCurrentSupplier;  //this is i-agent
		int mInteractionIndex{ 0 };
		double mCycleTempHonestTraectory{ 0 };
		double mCycleTempStategicTraectory{ 0 };
		double mCycleTempOutflow{ 0 };
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
		std::map <int,double> mReportedAverage;
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
		void ApplyNewTrustLevels();
		void PreserveCycle();
		double CalculateRecipientTresholdValue();
		double CalculateSupplierTresholdValue();
		double CalculateHonestPolicy(double trustLevel, double goodWill);
		double CalculateHonestRecipient(double gij, double pij, double trustLevel, float goodWill);
		AgentsFactory mAgentsFactory;
		CycleFactory mCycleFactory;
		bool mGeneratingReportDone{ false };
	public:
		int cyclesAmount{ 3 };
		int agentsAmount{ 1000 };
		int strategicAgentsAmount{ 50 };
		int kMin{ 50 };
		int kMax{ 150 };
		double expoA{ 0.5 };
		double expoG{ 0.5 };
		bool Done{ false };
		double beginTrustMesaure{ 1 };
		bool boostMode{ true };

		/// <summary>
		/// Reporing
		/// </summary>
		std::vector<float> SAgentTraectoryAvg;
		std::vector<float> HAgentTraectoryAvg;
		std::vector<float> NettoOutflow;
		std::vector<float> FinalTrust;
		std::vector<float> CycleNumbersForPlot;

		RaeLogger* logger;
		GoodWill goodWill{ GoodWill() };
		MonteCarlo(RaeLogger* logger);
		void Start();
		void Update();
		int CurrentRecipientNumberInCycle() { return mCurrentRecipient.Number+1; }
		int CurrentCycleNumber() { return mCurrentCycle.Round; }
		bool IsWorking(){ return mIsRunning || mIsInitalizing; }
		void GenerateReport();
		std::vector<float> GetTrustLevels();
	};


}