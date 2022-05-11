#pragma once
#include <GraphSetup.h>
#include <HelperFunctions.h>

void savecycles(std::vector<std::vector<size_t>>& cycles, string filename) {
	ofstream myfile;
	myfile.open(filename, std::ofstream::trunc);
	string contents;

	for each (std::vector<size_t> cycle in cycles)
	{
		stringstream buffer;
		for each (size_t var in cycle) {
			buffer << (int)var << " ";
		}
		contents = buffer.str();
		contents.pop_back();
		myfile << contents << endl;
		buffer.clear();
	}
	myfile.close();
}

void loadcycles(Graph* g, std::vector<std::vector<size_t>>& cycles, string filename) {
	cycles.clear();

	std::ifstream MyReadFile(filename);
	std::string line;
	while (std::getline(MyReadFile, line)) {
		std::stringstream ss(line);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> vstrings(begin, end);
		vector<size_t> cycle;

		if (!vstrings.empty()) {
			for (auto strin = vstrings.begin();
				strin != vstrings.end();
				strin++) {
				//console() << *strin << endl;
				cycle.push_back((size_t)stoi(*strin));
			}
			cycles.push_back(cycle);
		}
	}
	addCyclesToVertices(g, cycles);
}