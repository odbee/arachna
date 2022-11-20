#pragma once
#include "HelperFunctions.h"
#include <iostream>
#include <fstream>

#include <boost/graph/tiernan_all_cycles.hpp>

template<typename T>
inline std::vector<T> erase_indices(const std::vector<T>& data, std::vector<size_t>& indicesToDelete/* can't assume copy elision, don't pass-by-value */)
{
	if (indicesToDelete.empty())
		return data;

	std::vector<T> ret;
	ret.reserve(data.size() - indicesToDelete.size());

	std::sort(indicesToDelete.begin(), indicesToDelete.end());

	// new we can assume there is at least 1 element to delete. copy blocks at a time.
	std::vector<T>::const_iterator itBlockBegin = data.begin();
	for (std::vector<size_t>::const_iterator it = indicesToDelete.begin(); it != indicesToDelete.end(); ++it)
	{
		std::vector<T>::const_iterator itBlockEnd = data.begin() + *it;
		if (itBlockBegin != itBlockEnd)
		{
			std::copy(itBlockBegin, itBlockEnd, std::back_inserter(ret));
		}
		itBlockBegin = itBlockEnd + 1;
	}

	// copy last block.
	if (itBlockBegin != data.end())
	{
		std::copy(itBlockBegin, data.end(), std::back_inserter(ret));
	}

	return ret;
}

vector<vec3> getVertsFromFile(string filename) {
	ifstream MyReadFile(filename);
	string line;
	vector<vec3> result;
	vec3 cachevec = {};
	size_t counter = 0;
	while (getline(MyReadFile, line)) {
		counter = 0;
		std::string s = line;
		std::string delimiter = ",";
		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			if (counter == 0) {
				cachevec.x = stof(token);
			} else if(counter == 1) {
				cachevec.y = stof(token);
			}
			s.erase(0, pos + delimiter.length());
			counter++;
		}
		cachevec.z = stof(s);

		result.push_back(cachevec);

	}
	return result;
}







void InitialWebFromObj(Graph* g, float rc, string filename, std::vector<std::vector<size_t>>& cycs) {
	bool hasCycle;
	ifstream MyReadFile(filename);
	string line;
	while (getline(MyReadFile, line)) {
		std::stringstream ss(line);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> vstrings(begin, end);
		if (!vstrings.empty()) {
			if (vstrings[0] == "v") {
				auto a = boost::add_vertex(*g);
				position[a] = { stof(vstrings[1]),stof(vstrings[2]),stof(vstrings[3]) };
				if (vstrings.size() >= 5) {
					fixedBool[a]=stof(vstrings[4]); //
				}
				else {
					fixedBool[a] = true;
				}
				webLogger.addVertexLog(&a);
				
				
			}
			if (vstrings[0] == "l") {
				edge_t e = connectAB(g, stoi(vstrings[1]) - 1, stoi(vstrings[2]) - 1, rc,0, stoi(vstrings[3]));
				forbiddenPm[e] = stoi(vstrings[3]);
			}
		}

	}

	//boost::tiernan_all_cycles(g, vis);
	

	ci::app::console() << stringfromCycles(cycs) << endl;

}
