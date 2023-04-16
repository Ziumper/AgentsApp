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

//TEST(SupplierRandomizerTests, IsReturningEvenDistribute) {
//	auto randomizer = 
//
//	auto randomInts = randomizer.getEvenDistributeRandom();
//
//	//count each of the even distribute;
//
//	EXPECT_EQ(0, 100);
//}

TEST(SupplierRandomizerTests, CanGetAllNumbersFromRangeOneToSix) {

	auto loopEnd = 6;
	auto randomizer = Rae::SupplierRandomizer(1, loopEnd);

	auto randomInts = randomizer.getAllNumbersFromRangeOnce();

	//count each 
	auto counterMap = std::map<int, int>();
	auto sum = 0;

	for (int i = 1; i <= loopEnd; i++) {
		counterMap[i] = 0;

		for (auto& num : randomInts)
		{
			if (i == num) {
				counterMap[i] += 1;
			}
		}

		sum += counterMap[i];
	}

	EXPECT_EQ(sum, 6);
}