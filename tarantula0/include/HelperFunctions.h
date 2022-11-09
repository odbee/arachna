#pragma once

#include "cinder/app/App.h"
#include "GraphSetup.h"
#include <vector>
#include <string>


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <random>


template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

float getProbablityFromParams(std::vector<std::array<float, 2>>& paramlist, float length);

//https://stackoverflow.com/questions/5225820/compare-two-vectors-c
// rethink usage of const vector<size_t>
std::vector<size_t> compareVectorsReturnIntersection(const std::vector<size_t> vec1, const std::vector<size_t> vec2);



//
//
//
//
ci::Color cRamp(double ratio);
//
ci::vec3 getClosestPointFromList(ci::vec3 startPoint, std::vector<ci::vec3> listOfPoints);

ci::Color getColorFromInt(int val);

namespace std {
	std::string to_string(ci::vec3 vector);
}

ci::vec3 interpolateBetweenPoints(ci::vec3 point1, ci::vec3 point2, float t);

std::string stringfromCycles(cycleList cycles);

std::string stringfromCyclesShort(std::vector<size_t> cycleslist);


template <typename T>
std::string stringfromVec(std::vector<T> vec);