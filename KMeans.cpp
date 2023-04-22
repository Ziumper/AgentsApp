#include <vector>
#include "KMeans.h"
#include "Randomizer.h"

KMeans::KMeans(int amount, std::map<int, double> values) : mCentroidsAmount(amount) 
{
	this->mValues = values;
};

std::vector<Centroid> KMeans::CreateCentroids()
{
	mCentroids.clear();

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
	int max = GetMaxIndexFromValues();
	Randomizer randomizer = Randomizer(mValues[mValues.begin()->first], max);
	std::vector<double> points;

	for (int i = 0; i < this->mCentroidsAmount; i++) {
		int indexRandom = randomizer.GetEvenRandomNumber();
		double randomStartingPoint = mValues[indexRandom];
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

	std::map<int, double>::iterator it;

	for (it = mValues.begin(); it != this->mValues.end(); it++) {
		double checkValue = it->second;
		if (checkValue > max) {
			max = checkValue;
		}
	}

	return max;
}

int KMeans::GetMaxIndexFromValues() {

	int max = 0;

	std::map<int, double>::iterator it;

	for (it = mValues.begin(); it != this->mValues.end(); it++) {
		double checkValue = it->first;
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

	bool isNotTheSameAsPrevious = false;
	do
	{
		CreateCentroids();
		CountDistances();
		AssignPoints();
	} while (this->IsAnyCentroidEmpty());

	do 
	{
		CountAverage();
		CountDistances();
		AssignPoints();

		isNotTheSameAsPrevious = !this->IsTheSameAsPreviousAssign();
		AssignToTemp();
	} while (isNotTheSameAsPrevious);
	
}

void KMeans::AssignToTemp() {
	std::vector<Centroid>::iterator itC;

	//reset temp
	mAssignTemp.clear();

	for (itC = this->mCentroids.begin(); itC != this->mCentroids.end(); itC++) {
		mAssignTemp[itC->Number] = itC->Assigned;
	}
}

void KMeans::CountDistances() {
	for (Centroid& centroid : this->mCentroids) {
		centroid.Distances = this->CountDistances(centroid.Value);
	}
}

void KMeans::AssignPoints() {
	std::map<int, double>::iterator it;

	//cleanup the centroid assings first
	for (Centroid& centroid : mCentroids) {
		centroid.Assigned.clear();
	}

	//assign new ones
	for (it = this->mValues.begin(); it != this->mValues.end(); it++) {

		int index = this->GetMinCentroid(it->first);
		this->mCentroids[index].Assigned.push_back(it->first);
	}
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

void KMeans::CountAverage() {

	for (Centroid& centroid : mCentroids)
	{
		double average = 0;
		double sum = 0;

		int counter = 0;

		for (int& assigned : centroid.Assigned) {
			sum = sum + mValues[assigned];
			counter++;
		}

		centroid.Value = sum / counter;
	}
}
bool KMeans::IsTheSameAsPreviousAssign()
{
	for (Centroid& centroid : mCentroids) {
		if (centroid.Assigned.size() > mAssignTemp[centroid.Number].size()) return false;
		bool isEqual = std::equal(centroid.Assigned.begin(), centroid.Assigned.end(), mAssignTemp[centroid.Number].begin());
		if (isEqual == false) return false;
	}

	return true;
}
bool KMeans::IsAnyCentroidEmpty()
{
	if (this->mCentroids.size() == 0) return true;

	for (Centroid& centroid : this->mCentroids) {
		if (centroid.Assigned.size() == 0) return true;
	}

	return false;
}
;

bool Centroid::IsAssigned(int number)
{
	for (int& numberAssigned : Assigned) {
		if (number == numberAssigned) {
			return true;
		}
	}

	return false;
}

