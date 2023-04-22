#pragma once
#include <vector>
#include <map>




class Centroid {
public:
	int Number;
	std::map<int, double> Distances;
	double Value;
	std::vector<int> Assigned;
};


class KMeans {
private:
	int mCentroidsAmount;
	std::map<int, double> mValues;
	std::vector<Centroid> mCentroids;
	std::map<int, int> mAssignTemp;
public:
	KMeans(int amount, std::map<int, double> values);
	void SetCentroids(std::vector<Centroid> centroids) { this->mCentroids = centroids; };
	std::vector<Centroid> GetCentroids() { return this->mCentroids; }
	std::map<int, double> GetValues() { return this->mValues;  }
	std::vector<Centroid> CreateCentroids();
	std::vector<double> GetRandomStartingPoints();
	double GetMaxFromValues();
	static bool IsTwoDoubleEqual(double first, double second);
	std::map<int,double> CountDistances(double point);
	void ProcessKMeansClusterization();
	void CountDistances();
	bool AssignPoints();
	int GetMinCentroid(int pointIndex);
	double CountAverage();
	bool IsTheSameAsPreviousAssign();
};
