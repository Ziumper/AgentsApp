#include "pch.h"
#include "../Randomizer.h"
#include <vector>
#include "../KMeans.h"
#include "../Twitch.h"
#include "../CSVReader.h"
#include <iostream>
#include <fstream>
#include <filesystem>

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

void TestKMeansSeperated(KMeans kMeans) {
	kMeans.ProcessKMeansClusterization();

	std::vector<Centroid> centroids = kMeans.GetCentroids();

	for (size_t i = 0; i < centroids.size(); i++) {
		for (size_t j = 0; j < centroids.size(); j++)
			if (i != j) {
				std::vector<int> assignedSource = centroids[i].Assigned;
				std::vector<int> assingedSourceTwo = centroids[j].Assigned;

				for (int& assinged : assignedSource) {
					for (int& secondAssigned : assingedSourceTwo) {
						bool notEqual = assinged != secondAssigned;
						EXPECT_TRUE(notEqual);
					}
				}
			}
	}
}

static std::string GetPathToFile(std::string filename) {
	
	std::string executionRootPath = std::filesystem::current_path().string();
	std::string filePath = executionRootPath + "\\..\\..\\" + filename;
	return filePath;
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

TEST(KMeansTest, DoesntAddAnyMoreStandardPointsToValuesWhileCheckingMaxValues) {
	KMeans kMeans = GetTestKMeans();
	
	double max = kMeans.GetMaxFromValues();
	std::map<int, double> values = kMeans.GetValues();

	std::map<int, double>::iterator it; 

	for (it = values.begin(); it != values.end(); it++) {
		EXPECT_GT(it->first, 0);
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


TEST(KMeansTest, CanAssignPointsCorrectly) {
	//arrange
	KMeans kMeans = GetBasicTestKMeans();
	std::vector<int> firstPointsGroup = { 1,2 };
	std::vector<int> secondPointsGroup = { 3,4,5 };

	kMeans.CountDistances();
	kMeans.AssignPoints();

	std::vector<Centroid> centroids = kMeans.GetCentroids();
	std::vector<int> fromCentroid1 = centroids[0].Assigned;
	std::vector<int> fromCentroid2 = centroids[1].Assigned;
	
	bool checkFirst = std::equal(fromCentroid1.begin(), fromCentroid1.end(), firstPointsGroup.begin());
	bool checkSecond = std::equal(fromCentroid2.begin(), fromCentroid2.begin(), secondPointsGroup.begin());

	EXPECT_TRUE(checkFirst);
	EXPECT_TRUE(checkSecond);
}

TEST(KMeansTest, CountAverageCorrectly) {
	KMeans kMeans = GetBasicTestKMeans();

	kMeans.CountDistances();
	kMeans.AssignPoints();

	kMeans.CountAverage();

	std::vector<Centroid> centroids = kMeans.GetCentroids();

	EXPECT_DOUBLE_EQ(1.5,centroids[0].Value);
	EXPECT_DOUBLE_EQ(4,centroids[1].Value);
}

TEST(KMeansTest, CanProcessClusterizationCorrectlyForBasicTest) {
	KMeans kMeans = GetBasicTestKMeans();

	TestKMeansSeperated(kMeans);
}

TEST(KMeansTest, IsAssignTheSameAsPreviousOne) {
	KMeans kMeans = GetBasicTestKMeans();

	kMeans.CountDistances();
	kMeans.AssignPoints();
	kMeans.AssignToTemp();
	kMeans.CountAverage();
	kMeans.AssignPoints();
	kMeans.AssignToTemp();

	EXPECT_TRUE(kMeans.IsTheSameAsPreviousAssign());
}

TEST(KMeansTest, IsAssignTempTheSameSizeAsCentroidsAssign) {
	KMeans kMeans = GetBasicTestKMeans();

	kMeans.CountDistances();
	kMeans.AssignPoints();
	std::vector<Centroid> firstAssignCentroids = kMeans.GetCentroids();

	kMeans.CountAverage();
	kMeans.AssignPoints();

	std::vector<Centroid> secondAssignedCentroids = kMeans.GetCentroids();

	EXPECT_EQ(firstAssignCentroids[0].Assigned.size(),secondAssignedCentroids[0].Assigned.size());
	EXPECT_EQ(firstAssignCentroids[1].Assigned.size(), secondAssignedCentroids[1].Assigned.size());
}

TEST(KMeansTest, CanDo1000PointsIntoTwoCentroidsGroup) {
	//1. 1000 thousnd of random points in range [0,1]
	std::map<int, double> map;
	RealRandomizer randomizer = RealRandomizer(0,1);

	for (int i = 0; i < 1000; i++) {
		map[i] = randomizer.GetEvenRandomNumber();
	}

	//2. Create KMeans object
	KMeans kMeans = KMeans(2, map);

	//3. Test 
	TestKMeansSeperated(kMeans);
}

TEST(KMeansTest, CanGoWithEmptyCentroidAssing) {
	std::map<int, double> map = { {3,3}, {4,4}, {5,5} };
	KMeans kMeans = KMeans(2, map);

	std::vector<Centroid> centroids = kMeans.CreateCentroids();

	centroids[0].Value = 1;
	centroids[1].Value = 3;

	kMeans.SetCentroids(centroids);

	kMeans.ProcessKMeansClusterization();

	centroids = kMeans.GetCentroids();

	bool notEmpty = centroids[0].Assigned.size() != 0;
	EXPECT_TRUE(notEmpty);

	notEmpty = centroids[1].Assigned.size() != 0;
	EXPECT_TRUE(notEmpty);

}

TEST(KMeansTest, IsAnyCentroidEmtpy) {
	std::map<int, double> map = { {3,3}, {4,4}, {5,5} };
	KMeans kMeans = KMeans(2, map);
	
	std::vector<Centroid> centroids = { Centroid(), Centroid() };
	kMeans.SetCentroids(centroids);
	
	EXPECT_TRUE(kMeans.IsAnyCentroidEmpty());
}



TEST(CSVReaderTests, IsReadingFile) {
	
	std::string filename = "test.csv";
	std::ifstream rfile(GetPathToFile(filename));
	
	EXPECT_TRUE(rfile);
}

TEST(CSVReaderTests, IsReadingCSVFile) {
	CSVReader reader = CSVReader();
	std::string filename = "test.csv";
	std::ifstream rfile(GetPathToFile(filename));
	
	auto result = reader.ReadCSV(rfile);
	EXPECT_GT(result.size(), 0);
}

TEST(CSVReaderTests,IsReadingCSVFileFromStringView) {
	CSVReader reader = CSVReader();
	std::string path = GetPathToFile("test.csv");
	auto result = reader.ReadCSV(path);
	EXPECT_GT(result.size(), 0);
}

TEST(TwitchTests, IsReadingTwitchUsers) {
	Twitch twitch = Twitch();
	std::string pathToString = GetPathToFile("test.csv");
	auto result = twitch.ReadTwitchUserData(pathToString);
	EXPECT_GT(result.size(), 0);
}

TEST(TwitchTests, IsCreatingTwitchUserCorrectly) {
	Twitch twitch = Twitch();
	std::string pathToString = GetPathToFile("test.csv");
	CSVReader reader = CSVReader();
	auto result = reader.ReadCSV(pathToString);
	TwitchUser user = TwitchUser(result[1]);
	EXPECT_GT(user.Id, 0);
}

TEST(TwitchTests, IsCreatingTwitchUserWithStreamsCorrectly) {
	Twitch twitch = Twitch();
	std::string pathToString = GetPathToFile("test.csv");
	std::vector<std::string> pathsToStreamerFiles;
	pathsToStreamerFiles.push_back(GetPathToFile("testStreams.csv"));
	auto users = twitch.ReadTwitchUserData(pathsToStreamerFiles,pathToString);
	TwitchUser user = users[0];

	EXPECT_GT(user.Streams.size(), 0);
}

TEST(TwitchTests, IsCreatingTwitchStreamsCorrectlyFromDataPath) {
	Twitch twitch = Twitch();
	std::string pathToString = GetPathToFile("testStreams.csv");
	
	auto result = twitch.ReadTwitchStreams(pathToString);
	EXPECT_GT(result.size(), 0);
}