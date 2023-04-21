#include <vector>
#include "KMeans.h"
#include "Randomizer.h"

KMeans::KMeans(int amount, std::map<int, double> values) : mCentroidsAmount(amount), mValues(values) 
{
	 
};

std::vector<Centroid> KMeans::CreateCentroids()
{
	std::vector<double> startingPoints = GetRandomStartingPoints();

	for (int i = 0; i < mCentroidsAmount; i++) {
		Centroid centroid = Centroid();
		centroid.Number = i;
		double startingPoint = startingPoints[i];
		double average = startingPoint;
		centroid.Value = average;
		this->mCentroids.push_back(centroid);
	}

	return this->mCentroids;
}

std::vector<double> KMeans::GetRandomStartingPoints() {
	double max = GetMaxFromValues();
	RealRandomizer randomizer = RealRandomizer(0, max);
	std::vector<double> points;

	for (int i = 0; i < this->mCentroidsAmount; i++) {
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

std::map<int,double> KMeans::CountDistances(double point)
{
	std::map<int,double> distances;
	std::map<int, double>::iterator it;

	for (it = mValues.begin(); it != mValues.end(); it++)
	{
		double value = it->second;
		double distance = point - value;
		distance = std::abs(distance);
		distances[it->first]= distance;
	}

	return distances;
}

void KMeans::ProcessKMeansClusterization() {

	CreateCentroids();
	CountDistances();

	bool isTheSameAsPrevious = AssignPoints();

	do {
		CountAverage();
		CountDistances();
		isTheSameAsPrevious = AssignPoints();
	} while (isTheSameAsPrevious);
	
}

void KMeans::CountDistances() {
	for (Centroid& centroid : this->mCentroids) {
		centroid.Distances = this->CountDistances(centroid.Value);
	}
}

bool KMeans::AssignPoints() {
	std::map<int, double>::iterator it;

	for (it = this->mValues.begin(); it != this->mValues.end(); it++) {
		int index = this->GetMinCentroid(it->first);
		Centroid centroid = this->mCentroids[index];
		centroid.Assigned.push_back(it->first);
	}

	return this->IsTheSameAsPreviousAssign();
}
int KMeans::GetMinCentroid(int pointIndex)
{
	int min = 0;

	//it wouldn't be bigger 
	double minDistance = 100;

	for (Centroid& centroid : this->mCentroids) {
		if (centroid.Distances[pointIndex] < minDistance) {
			minDistance = centroid.Distances[pointIndex];
			min = centroid.Number;
		}
	}

	return min;
}

double KMeans::CountAverage() {
	return 0;
}
bool KMeans::IsTheSameAsPreviousAssign()
{
	return false;
}
;
