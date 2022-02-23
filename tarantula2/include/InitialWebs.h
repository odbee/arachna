#pragma once
#include "HelperFunctions.h"
#include <iostream>
#include <fstream>



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

void InitialWebFromPc(Graph* g,  float rc, string filename) {
	
	vector<vec3>verts = getVertsFromFile(filename);
	
	auto cvert = boost::add_vertex(*g);
	position[cvert]= {0, 0, 0};
	fixedBool[cvert] = false;

	for (const auto& vert : verts)
	{
		position[cvert]+= vert;

	}
	position[cvert] /= verts.size();
	
	for (const auto & vert: verts)
	{
		auto a = boost::add_vertex(*g);
		position[a] = vert;
		fixedBool[a] = true;
		connectAB(g, a, cvert, rc);
	}

}
void addVertsAndEdgesFromFile(string filename, Graph g) {

}

void InitialWebFromObj(Graph* g, float rc, string filename) {

	addVertsAndEdgesFromFile(filename, *g);
	vector<vec3>verts = getVertsFromFile(filename);

	auto cvert = boost::add_vertex(*g);
	position[cvert] = { 0, 0, 0 };
	fixedBool[cvert] = false;

	for (const auto& vert : verts)
	{
		position[cvert] += vert;

	}
	position[cvert] /= verts.size();

	for (const auto& vert : verts)
	{
		auto a = boost::add_vertex(*g);
		position[a] = vert;
		fixedBool[a] = true;
		connectAB(g, a, cvert, rc);
	}

}