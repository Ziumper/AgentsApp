#include "pch.h"
#include "../Rae.h"

TEST(ReputationTests, IsSupplierBehaviourAboveZero) {
	auto reputation = Rae::Reputation();
	reputation.setProvidedServices(2);
	reputation.setRequestedServices(3);

	EXPECT_EQ(reputation.getSuplierBehaviour(), 1);
}

TEST(ReputationTests, CanHandleBiggerNumbers) {
	auto reputation = Rae::Reputation();
	reputation.setProvidedServices(200);
	reputation.setRequestedServices(300);

	EXPECT_EQ(reputation.getSuplierBehaviour(), 100);
}


TEST(SupplierRandomizerTests, CanGetEvenDistributionFor1000RandomNumbers) {
	auto start = 50;
	auto end = 150;
	auto randomizer = Rae::SupplierRandomizer(start,end);
	
	//mean in even distribute is always equal to (a+b)/2
	double evenDistributeMean =  (end+start)/2;
	double variance = sqrt((end - start) * (end - start) / 12);
	double rangeBoundUp = evenDistributeMean + variance;
	double rangeBoundDown = evenDistributeMean - variance;

	auto randomInts = randomizer.getEvenDistribute(1000);
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