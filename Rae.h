#pragma once
#include <vector>
#include <memory>
#include <string_view>
#include <random>
#include <string>

namespace Rae
{
	class GoodWill {
	public:
		std::unique_ptr<double> x{ std::make_unique<double>(0) };
		std::unique_ptr<double> y{ std::make_unique<double>(0) };
		std::unique_ptr<double> z{ std::make_unique<double>(0) };
		GoodWill() {};
	};

	class RaeLogger {
	public:
		virtual void AddLog(const char *entry) = 0;
		virtual void AddLog(const char* entry, const int number) = 0;
		virtual ~RaeLogger() {};
	};

	class Rtbs {
	private: 
		RaeLogger *logger;
	public:
		std::unique_ptr<int> cycles{ std::make_unique<int>(3) };
		std::unique_ptr<int> agentsAmount{ std::make_unique<int>(1000) };
		std::unique_ptr<int> strategicAgentsAmount{ std::make_unique<int>(50) };
		std::unique_ptr<int> kMin{ std::make_unique<int>(50) };
		std::unique_ptr<int> kMax{ std::make_unique<int>(150) };
		std::unique_ptr<double> expoA{ std::make_unique<double>(0.5) };
		std::unique_ptr<double> expoG{ std::make_unique<double>(0.5) };
		std::unique_ptr<double> beginTrustMesaure{ std::make_unique<double>(1) };
		GoodWill goodWill{ GoodWill() };
		Rtbs(RaeLogger *logger): logger(logger) {}
		void StartMonteCarlo();
		void RunCycle(int cycleNumber);
	};

	class Cycle {
		std::unique_ptr<int> round;
	};

	class Agent {
	public:
		int number;
		double trust;
		Agent() {};
	};

	class Randomizer {
	private:
		std::unique_ptr<int> mMin;
		std::unique_ptr<int> mMax;
		std::uniform_int_distribution<> distributor;
		std::mt19937 generator;
	public:
		Randomizer(int min, int max);
		std::vector<int> GetEvenDistribute();
		std::vector<int> GetEvenDistribute(int amount);
		int GetEvenRandomNumber();
	};
}