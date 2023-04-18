#include "pch.h"
#include "../Rae.h"

TEST(SupplierRandomizerTests, CanGetEvenDistributionFor1000RandomNumbers) {
	auto start = 50;
	auto end = 150;
	auto randomizer = Rae::Randomizer(start,end);
	
	//mean in even distribute is always equal to (a+b)/2
	double evenDistributeMean =  (end+start)/2;
	double variance = sqrt((end - start) * (end - start) / 12);
	double rangeBoundUp = evenDistributeMean + variance;
	double rangeBoundDown = evenDistributeMean - variance;

	std::vector randomInts = randomizer.GetEvenDistribute(1000);
	auto sum = 0;

	for (auto& num : randomInts)
	{
		sum += num;
	}

	int length = randomInts.capacity();
	double calculatedMean = (sum / length);

	//if calcul
	EXPECT_LT(calculatedMean, rangeBoundUp);
	EXPECT_GT(calculatedMean, rangeBoundDown);
}