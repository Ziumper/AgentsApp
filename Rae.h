#pragma once
#include <vector>
#include <memory>
#include <string_view>
#include <random>
#include <string>

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
		virtual void AddLog(const char* entry, const int number) = 0;
		virtual void AddLog(const char* entry, const double number) = 0;
		virtual ~RaeLogger() {};
	};

	class Agent {
	public:
		int number{ 0 };
		double trust{ 0 };
		double serviceAvailiability{ 0 };
		double serviceReception{ 0 };
		bool isStrategicAgent{ false };
		bool wasRecipient{ false };
		int suppliersAmount{ 0 };
		std::vector<int> suppilerNumbers;
		void ResetForNextCycle() {
			suppliersAmount = 0; 
			serviceReception = 0;
			serviceAvailiability = 0;
		}

		void CopyValues(Agent agent);
	};

	class Cycle {
		std::vector<Agent> mAgents;
	public:
		int round{ 0 };
		Cycle(int roundNumber) : round(roundNumber) {};
		Cycle() : round(0) {};
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
		Agent Create(int number);
	};

	class CycleFactory {
	private:
		RaeLogger* mLogger{ 0 };
	public:
		CycleFactory() {};
		CycleFactory(RaeLogger *logger): mLogger(logger) {};
		Cycle Create(int number);
	};

	class MonteCarlo {
	private:
		Cycle mCurrentCycle;
		Agent mCurrentRecipient;
		Agent mCurrentSuppiler;
		std::vector<Agent> mAgents;
		std::vector<Cycle> mCycles;
		bool mIsRunning{ false };
		bool mIsInitalizing{ false };
		void SwitchToNextCycle();
		void SetServiceAvailiabilityForAgent(Agent* agent);
		void SetServiceReceptionForAgent(Agent* agent);
		int SetSuppilersAmountForRecipient();
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
		RaeLogger* logger;
		GoodWill goodWill{ GoodWill() };
		MonteCarlo(RaeLogger* logger);
		void Start();
		void Update();
		void Run();
		void Initialize();
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