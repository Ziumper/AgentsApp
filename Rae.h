#pragma once
#include <vector>
#include <memory>
#include <string_view>




namespace Rae 
{
	class Cycle {
		std::unique_ptr<int> round;
	};

	class Agent {
	public:
		std::string_view nickname;
	};

	class SupplierRandomizer {
	private:
		std::unique_ptr<int> mMin;
		std::unique_ptr<int> mMax;
	public:
		SupplierRandomizer();
		SupplierRandomizer(int min, int max);
		std::vector<int> getEvenDistributeRandom();
		std::vector<int> getAllNumbersFromRangeOnce();
		
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