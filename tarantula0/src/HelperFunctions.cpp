#include "HelperFunctions.h"



float getProbablityFromParams(std::vector<std::array<float, 2>>& paramlist, float length) {
	float result = 0.0f;
	int paramslength = paramlist.size();

	for (size_t i = 0; i < paramslength; i++)
	{
		auto param = (paramlist)[i];
		auto paramplus = (paramlist)[(i + 1) % paramslength];
		if (paramplus[0] < 0.001) {
			paramplus[0] = 1;
		}

		result += (paramplus[0] - param[0]) * param[1];

	}
	result *= length;
	return result;
}


std::vector<size_t> compareVectorsReturnIntersection(const std::vector<size_t> vec1, const std::vector<size_t> vec2) {
	std::set<size_t> s1(vec1.begin(), vec1.end());
	std::set<size_t> s2(vec2.begin(), vec2.end());
	std::vector<size_t> v3;
	std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(v3));
	return v3;
}


ci::Color cRamp(double ratio) {

	//we want to normalize ratio so that it fits in to 6 regions
//where each region is 256 units long
	ratio = std::clamp(ratio, double(0), double(10));
	ratio = ratio / 11;

	int normalized = int(ratio * 256 * 3);

	//find the distance to the start of the closest region
	int x = normalized % 256;

	float red = 0, grn = 0, blu = 0;
	switch (normalized / 256)
	{
	case 0: red = 0;        grn = 255;      blu = x;       break;//green
	case 1: red = 255 - x;  grn = 255;      blu = 0;       break;//yellow
	case 2: red = 255;      grn = x;        blu = 0;       break;//red
	}
	return ci::Color(red / 255, grn / 255, blu / 255);
}


ci::vec3 getClosestPointFromList(ci::vec3 startPoint, std::vector<ci::vec3> listOfPoints) {
	float mindist = std::numeric_limits<float>::max();
	ci::vec3 cpPt;
	float compdist = 0;
	for (ci::vec3 compPt : listOfPoints) {
		compdist = ci::distance(startPoint, compPt);
		if (compdist < mindist) {
			mindist = compdist;
			cpPt = compPt;
		}
	}
	return cpPt;
}

ci::Color getColorFromInt(int val) {
	float r = sin(val) / 2 + .5;
	float g = (cos(val)) / 2 + 0.5;
	float b = sin(2 * val) + cos(val) / 2 / 2 + .5;

	//ci::app::console() << "r " << r << "g  " << g << "b " << b << endl;

	return ci::Color(r, g, b);
}
namespace std {
	std::string to_string(ci::vec3 vector) {
		return std::to_string(vector.x) + "," + std::to_string(vector.y) + "," + std::to_string(vector.z);
	}
}


ci::vec3 interpolateBetweenPoints(ci::vec3 point1, ci::vec3 point2, float t) {
	return point1 + (point2 - point1) * t;
}

std::string stringfromCycles(cycleList cycles) {
	std::string result = "[ \n";

	for (const auto& cycle : cycles) {
		result.append(stringfromVec(cycle) + " \n");

	}
	result.append(" ]");
	return result;
}

std::string stringfromCyclesShort(std::vector<size_t> cycleslist) {
	std::string outtext;
	for (const auto& elem : cycleslist) {
		outtext.append(std::to_string(elem) + "  ");

	}
	return outtext;
}

template <typename T>
std::string stringfromVec(std::vector<T> vec) {
	std::string result = "[ ";
	for (auto it = vec.begin(); it != vec.end(); it++) {
		result.append(std::to_string(*it));
		result.append(" ");
	}
	result.append(" ]");
	return result;
}