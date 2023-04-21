#include <vector>
#include "KMeans.h"
#include "Randomizer.h"



std::vector<Cluster> KMeans::CreateClusers()
{
	std::vector<Cluster> clusters = std::vector<Cluster>();
	std::vector<int> startingPoints = GetRandomStartingPoints();

	for (int i = 0; i < mClustersAmount; i++) {
		Cluster cluster = Cluster();
		cluster.Number = i;
		int startingNumber = startingPoints[i];
		double average = mValues[startingNumber];
		cluster.AverageValue = average;
		clusters.push_back(cluster);
	}

	return clusters;
}

std::vector<int> KMeans::GetRandomStartingPoints() {

	//1. Get maximum from mValues double value
	Randomizer randomizer = Randomizer(0, mValues.size());

	std::vector<int> pointsIndexs;

	/*for (int i = 0; i < mClustersAmount; i++) {
		int pointIndex = randomizer.GetEvenRandomNumber();

		for (int& index : pointsIndexs) {
			double pointsIndex
		}
	}
	*/

	return pointsIndexs;
}
