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
		bool wasRecipent{ false };
	};

	class Cycle {
		std::vector<Agent> mAgents;
		Agent mRecipientAgent{ 0 };
	public:
		int round{ 0 };
		Cycle(int roundNumber) : round(roundNumber) {};
		Cycle() : round(0) {};
		bool IsDone();
		void SetAgents(std::vector<Agent> agents) { mAgents = agents; };
		std::vector<Agent> GetAgents() { return mAgents; }
		void Start();
	};

	template<typename T>
	class Factory {
	protected:
		int mCounter{ 0 };
		int mAmount{ 0 };
		std::vector<T> mObjects;
	public:
		Factory<T>(int amount) : mAmount(amount) {};
		std::vector<T> GetObjects() { return mObjects; }
		inline bool IsDone() {
			return mCounter >= mAmount;
		}
		inline virtual void Create() = 0;
	};

	class AgentsFactory: Factory<Agent> {
	private:
		double mBeginingTrustLevel;
		int mStrategicCount;
	public:
		AgentsFactory(int amount, double trustLevel,int strategic) : 
			Factory<Agent>(amount), 
			mBeginingTrustLevel(trustLevel),
			mStrategicCount(strategic) {};
		inline void Create() override;
	};

	class CycleFactory: Factory<Cycle> {
	private:
		std::vector<Agent> mAgents;
	public:
		CycleFactory(int amount, std::vector<Agent> agents) : Factory<Cycle>(amount), mAgents(agents) {};
		inline void Create() override;
	};

	class MonteCarlo {
	private:
		Cycle mCurrentCycle;
		std::vector<Agent> mAgents;
		std::vector<Cycle> mCycles;
		bool mIsRunning{ false };
		//void ChooseSuppilers(Cycle *cycle);
		void CreateAgents();
		void CreateCycles();
		void SwitchToNextCycle();
		void SetServiceAvailiabilityForAgent(Agent* agent);
		void SetServiceReceptionForAgent(Agent* agent);
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
		MonteCarlo(RaeLogger* logger) : logger(logger) {};
		void Start();
		void Update();
	};

	
	//TODO 
	/*
	* 1. Przerobiæ konstruktory 
	* 2. Przekopiowaæ referencje do tablicy z mCurrentCycle do mCycles
	* 3. To samo zrobiæ dla agentów - punkt 2 
	* 4. Update dla agenta i losowanie pojedyñczego agenta 
	*/
}