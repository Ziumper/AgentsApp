#include "pch.h"
#include "../Randomizer.h"
#include <vector>
#include "../KMeans.h"


TEST(SupplierRandomizerTests, CanGetEvenDistributionFor1000RandomNumbers) {
	auto start = 50;
	auto end = 150;
	auto randomizer = Randomizer(start,end);
	
	//mean in even distribute is always equal to (a+b)/2
	double evenDistributeMean =  (end+start)/2;
	double variance = sqrt((end - start) * (end - start) / 12);
	double rangeBoundUp = evenDistributeMean + variance;
	double rangeBoundDown = evenDistributeMean - variance;

	std::vector<int> randomInts = randomizer.GetEvenDistribute(1000);
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

static int amountClusters = 2;
static std::map<int, double> map = { {1,0.1},{2,0.2},{3,0.3} ,{4,0.4},{5,0.5} };
KMeans GetTestKMeans() {
	return KMeans(amountClusters, map);
}

TEST(KMeansTest, CanGetMaxFromValues) {
	double max = map[map.size()];

	KMeans testing = GetTestKMeans();
	double maxFromTests = testing.GetMaxFromValues();

	EXPECT_DOUBLE_EQ(max, maxFromTests);
}