#pragma once
#include <vector>
#include <map>




class Cluster {
public:
	int Number;
	std::map<int, double> Values;
	double AverageValue;
};


class KMeans {
private:
	int mClustersAmount;
	std::map<int, double> mValues;
public:
	KMeans(int amount, std::map<int, double> values) : mClustersAmount(amount), mValues(values) {};
	std::vector<Cluster> CreateClusers();
	std::vector<double> GetRandomStartingPoints();
	double GetMaxFromValues();
};
