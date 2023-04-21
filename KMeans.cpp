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
		//int startingNumber = startingPoints[i];
		double average = 0;
		cluster.AverageValue = average;
		clusters.push_back(cluster);
	}

	return clusters;
}

std::vector<double> KMeans::GetRandomStartingPoints() {

	//1. Get maximum from mValues double value
	double max = GetMaxFromValues();
	RealRandomizer randomizer = RealRandomizer(0, max);
	std::vector<double> points;

	for (int i = 0; i < this->mClustersAmount; i++) {
		double randomStartingPoint = randomizer.GetEvenRandomNumber();
		if (points.size() > 0) {
			//check if it's already inside points
			for (double &point : points) {
				while (randomStartingPoint == point) {
					randomStartingPoint = randomizer.GetEvenRandomNumber();
				}
			}
		}

		//add 
		points.push_back(randomStartingPoint);
	}

	return points;
}

double KMeans::GetMaxFromValues() {

	double max = 0;

	for (int i = 0; i < mValues.size(); i++) {
		double checkValue = mValues[i];
		if (checkValue > max) {
			max = checkValue;
		}
	}

	return max;
}
