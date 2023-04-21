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

static int amountOfCentroids = 2;
static std::map<int, double> map = { {1,0.1},{2,0.2},{3,0.3} ,{4,0.4},{5,0.5} };
KMeans GetTestKMeans() {
	return KMeans(amountOfCentroids, map);
}

KMeans GetBasicTestKMeans() {
	std::map<int, double> testPointsMap = { { 1,1.0 }, { 2,2.0 }, { 3,3.0 }, { 4,4.0 }, { 5,5.0 } };
	
	auto kMeans = KMeans(2, testPointsMap);



	std::vector<Centroid> centroids = kMeans.CreateCentroids();
	centroids[0].Value = 0.5; // first 
	centroids[1].Value = 3.5; // second

	kMeans.SetCentroids(centroids);
	return kMeans;
}

TEST(KMeansTest, CanGetMaxFromValues) {
	double max = map[map.size()];

	KMeans testing = GetTestKMeans();
	double maxFromTests = testing.GetMaxFromValues();

	EXPECT_DOUBLE_EQ(max, maxFromTests);
}


TEST(KMeansTest, CanRequiredClustersSize) {
	KMeans kMeans = GetTestKMeans();
	EXPECT_EQ(amountOfCentroids,kMeans.CreateCentroids().size());
}

TEST(KMeansTest, CanGetTwoDifferentRandomPoints) {
	KMeans kMeans = GetTestKMeans();

	for (int i = 0; i < 1000; i++) {
		std::vector<double> points = kMeans.GetRandomStartingPoints();
		for (int j = 0; j < points.size(); j++) {
			for (int k = 0; k < points.size(); k++) {
				if (j != k) {
					double first = points[j];
					double second = points[k];
					
					bool equal = KMeans::IsTwoDoubleEqual(first, second);

					EXPECT_FALSE(equal);
				}
			}
		}
	}
}

TEST(KMeansTest, CentroidHaveNotZeroStartingPoints) {
	KMeans kMeans = GetTestKMeans();
	std::vector<Centroid> centroids = kMeans.CreateCentroids();

	for (Centroid& centroid : centroids) {
		EXPECT_FALSE(KMeans::IsTwoDoubleEqual(0, centroid.Value));
	}
}

TEST(KMeansTest, IsEqualDouble) {
	double first = 0.1312312123131;
	double second = first;

	bool equal = KMeans::IsTwoDoubleEqual(first,second);

	EXPECT_TRUE(equal);
}

TEST(KMeansTest, CanGetValidDistancesForPoint) {
	std::map<int,double> testPointsMap = { { 1,1.0 }, { 2,2.0 }, { 3,3.0 }, { 4,4.0 }, { 5,5.0 } };
	auto kMeans = KMeans(2, testPointsMap);
	std::map<int, double> valid = { {1,0.5}, {2,1.5}, {3,2.5}, {4,3.5}, {5,4.5} };
	double point = 0.5;
	std::map<int,double> calculated = kMeans.CountDistances(point);
	std::map<int, double>::iterator it; 

	for (it = testPointsMap.begin(); it != testPointsMap.end(); it++) {
		EXPECT_DOUBLE_EQ(valid[it->first], calculated[it->first]);
	}
}

TEST(KMeansTest, CanGetMinCentroid) {

	//arrange
	int validCentroidIndex = 0;
	int pointIndexToCheck = 1; // {1, 1.0} distance = 0.5
	auto kMeans = GetBasicTestKMeans();

	//act
	
	kMeans.CountDistances();
	int centroidIndex = kMeans.GetMinCentroid(pointIndexToCheck);


	//assert
	EXPECT_EQ(validCentroidIndex, centroidIndex);
}