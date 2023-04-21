#include <vector>
#include "KMeans.h"
#include "Randomizer.h"



std::vector<Cluster> KMeans::CreateClusers()
{
	std::vector<Cluster> clusters = std::vector<Cluster>();
	std::vector<double> startingPoints = GetRandomStartingPoints();

	for (int i = 0; i < mClustersAmount; i++) {
		Cluster cluster = Cluster();
		cluster.Number = i;
		double startingPoint = startingPoints[i];
		double average = startingPoint;
		cluster.AverageValue = average;
		clusters.push_back(cluster);
	}

	return clusters;
}

std::vector<double> KMeans::GetRandomStartingPoints() {
	double max = GetMaxFromValues();
	RealRandomizer randomizer = RealRandomizer(0, max);
	std::vector<double> points;

	for (int i = 0; i < this->mClustersAmount; i++) {
		double randomStartingPoint = randomizer.GetEvenRandomNumber();
		if (points.size() > 0) {
			//check if it's already inside points
			for (double &point : points) {
				while (IsTwoDoubleEqual(randomStartingPoint,point)) {
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

bool KMeans::IsTwoDoubleEqual(double first, double second) {
	return !(std::isgreater(first, second) || std::isgreater(second, first));
}

std::vector<double> KMeans::CountDistances(double point)
{
	std::vector<double> distances;
	std::map<int, double>::iterator it;

	for (it = mValues.begin(); it != mValues.end(); it++)
	{
		double value = it->second;
		double distance = point - value;
		distance = std::abs(distance);
		distances.push_back(distance);
	}

	return distances;
}
