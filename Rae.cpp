#include "Rae.h"
#include <vector>

namespace Rae {

	std::vector<int> SupplierRandomizer::getEvenDistributeRandom()
	{
		return std::vector<int>();
	}

	std::vector<int> SupplierRandomizer::getAllNumbersFromRangeOnce()
	{
		auto numbers = std::vector<int>();

		int end = *mMax.get();
		int start = *mMin.get();

		for (int i = start; i <= end; i++) {
			numbers.push_back(i);
		}

		return numbers;
	}

	SupplierRandomizer::SupplierRandomizer(int min, int max):
		mMax(std::make_unique<int>(max)),
		mMin(std::make_unique<int>(min))
	{};

	SupplierRandomizer::SupplierRandomizer():
		mMax(std::make_unique<int>(150)),
		mMin(std::make_unique<int>(50))
	{};


		
	

}

