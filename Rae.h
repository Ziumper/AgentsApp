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
	public:
		std::vector<Agent> agents;
		std::vector<Agent> suppilers;
		Agent recipent;
		int round{ 0 };
		Cycle(int roundNumber) : round(roundNumber) {};
	};

	class MonteCarlo {
	private:
		void RunCycle(Cycle *cycle);
		void ChooseSuppilers(Cycle *cycle);
		std::vector<Agent> CreateAgents();
		std::vector<Cycle> CreateCycles();
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
	};

	
}