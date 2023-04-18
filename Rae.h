#pragma once
#include <vector>
#include <memory>
#include <string_view>
#include <random>

namespace Rae
{
	class GoodWill {
	public:
		std::unique_ptr<double> x{ std::make_unique<double>(0) };
		std::unique_ptr<double> y{ std::make_unique<double>(0) };
		std::unique_ptr<double> z{ std::make_unique<double>(0) };
		GoodWill() {};
	};

	class Rtbs {
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
		Rtbs() {};
	};

	

	class Cycle {
		std::unique_ptr<int> round;
	};

	class Agent {
	public:
		std::string_view nickname;
	};

	class Randomizer {
	private:
		std::unique_ptr<int> mMin;
		std::unique_ptr<int> mMax;
		std::uniform_int_distribution<> distributor;
		std::mt19937 generator;
	public:
		Randomizer(int min, int max);
		std::vector<int> getEvenDistribute();
		std::vector<int> getEvenDistribute(int amount);
		int getEvenRandomNumber();
	};

	class Reputation {
	private:
		std::unique_ptr<int> mProvidedServices;
		std::unique_ptr<int> mRequestedServices;
	public:
		/// <summary>
		/// An agent attribute that dictates to other agents behavior in interactions with it,
		/// </summary>
		std::unique_ptr<int> trustSize;
		/// <summary>
		/// Aggregate assessment of the agent by others dictated by his past behavior
		/// </summary>
		std::unique_ptr<int> reputation;
		/// <summary>

		/// </summary>
		std::unique_ptr<int> supplierBehaviour;
		/// <summary>
		/// Number of services reported to RAE as received (so-called reported services or reputation data),
		/// </summary>
		std::unique_ptr<int> recipientBehaviour;

		/// <summary>
		/// rules specifying the number of services provided and reported
		/// </summary>
		std::vector<int> behaviourPolicy;

		/// <summary>
		/// The number of services provided in relation to the amount requested (so-called services provided),
		/// </summary>
		/// <returns></returns>
		int getSuplierBehaviour() {
			int provided = *mProvidedServices.get();
			int requested = *mRequestedServices.get();
			return requested - provided;
		};

		void setProvidedServices(int provided) { mProvidedServices = std::make_unique<int>(provided); }
		void setRequestedServices(int requested) { mRequestedServices = std::make_unique<int>(requested); }
	};
}