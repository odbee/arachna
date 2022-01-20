#pragma once
#include "GraphSetup.h"
#include <random>



string stringfromVec(vector<size_t> vec) {
	string result = "[ ";
	for (auto it = vec.begin(); it != vec.end(); it++) {
		result.append(to_string(*it));
		result.append(" ");
	}
	result.append(" ]");
	return result;
}
string stringfromCycles(std::vector<std::vector<size_t>> cycles) {
	string result = "[ \n";

	for (const auto& cycle : cycles) {
		result.append(stringfromVec(cycle) + " \n");

	}
	result.append(" ]");
	return result;
}

string stringfromCyclesShort(std::vector<size_t> cycleslist) {
	string outtext;
	for (const auto& elem : cycleslist) {
		outtext.append(to_string(elem) + "  ");

	}
	return outtext;
}



edge_t connectAB(Graph* g, Graph::vertex_descriptor endPointA, Graph::vertex_descriptor endPointB, float rc, int ind =0, bool isforbidden=false) {
	float dd = distance(position[endPointA], position[endPointB]);
	edge_t edge = boost::add_edge(endPointA, endPointB, *g).first;
	currentLengthPm[edge] = dd;
	restLengthPm[edge] = dd * rc;
	forbiddenPm[edge] = isforbidden;
	if (ind == 0) {
		indexPm[edge] = ++GLOBALINT;
	}
	else {
		indexPm[edge] = ind;
	}
	return edge;
}

pair<edge_t, bool> connectAB2(Graph* g, Graph::vertex_descriptor endPointA, Graph::vertex_descriptor endPointB, float rc, int ind = 0, bool isforbidden = false) {
	float dd = distance(position[endPointA], position[endPointB]);
	edge_t edge;
	bool result;
	auto res= boost::add_edge(endPointA, endPointB, *g);
	edge = res.first;

	currentLengthPm[edge] = dd;
	restLengthPm[edge] = dd * rc;
	forbiddenPm[edge] = isforbidden;
	if (ind == 0) {
		indexPm[edge] = ++GLOBALINT;
	}
	else {
		indexPm[edge] = ind;
	}
	return res;
}




std::optional<edge_t> GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g)
{
	for (auto e : boost::make_iterator_range(out_edges(v, g))) {
		if (target(e, g) == u)
			return e;
	}
	return {};
	throw std::domain_error("my_find_edge: not found");
}


void addCyclesToVertices(Graph* g, std::vector<std::vector<size_t>> cycles) {
	for (int i = 0; i < cycles.size(); i++)
	{
		for (const auto& elem : cycles[i])
			cyclesPm[elem].push_back(i);
	}
}

void emptyCyclesFromVertices(Graph* g) {
	for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {
		cyclesPm[*vi].clear();
	}
}


//https://stackoverflow.com/questions/5225820/compare-two-vectors-c
// rethink usage of const vector<size_t>
vector<size_t> compareVectorsReturnIntersection(const vector<size_t> vec1, const vector<size_t> vec2) {
	std::set<size_t> s1(vec1.begin(), vec1.end());
	std::set<size_t> s2(vec2.begin(), vec2.end());
	std::vector<size_t> v3;
	std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(v3));
	return v3;
}
edge_ti getRandomEdgeWeightedByLength(Graph* g) {

	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	int iteratorLength = 0;
	int randiter;
	bool isforbidden = true;
	float fulllength = 0;


	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		iteratorLength++;
		fulllength += currentLengthPm[*ei];
	}
	std::uniform_real_distribution<> dis(0.0, fulllength);
	
	edge_ti ei_startEdge;

	while (isforbidden)
	{
		auto randn = dis(gen);
		for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
			if (randn < currentLengthPm[*ei]) {
				ei_startEdge = ei;
				break;
			}
			randn -= currentLengthPm[*ei];
		}


		isforbidden = forbiddenPm[*ei_startEdge] && CHECKFORBIDDEN;
		//isforbidden = false;
	}
	

	return ei_startEdge;
}

edge_ti getRandomEdgeWeightedByTension(Graph* g) {
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	int iteratorLength = 0;
	int randiter;
	bool isforbidden = true;
	float fulllength = 0;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		iteratorLength++;
		fulllength += restLengthPm[*ei]/currentLengthPm[*ei];
	}
	std::uniform_real_distribution<> dis(0.0, fulllength);

	edge_ti ei_startEdge;

	while (isforbidden)
	{
		auto randn = dis(gen);
		for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
			if (randn < restLengthPm[*ei] / currentLengthPm[*ei]) {
				ei_startEdge = ei;
				break;
			}
			randn -= restLengthPm[*ei] / currentLengthPm[*ei];
		}


		isforbidden = forbiddenPm[*ei_startEdge] && CHECKFORBIDDEN;
		//isforbidden = false;
	}
	return ei_startEdge;
}


edge_ti getRandomEdgeWeightedByX(Graph* g) {
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	int iteratorLength = 0;
	int randiter;
	bool isforbidden = true;
	float fulllength= 0;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		iteratorLength++;
		fulllength+=pow(((position[boost::source(*ei, *g)].y + position[boost::target(*ei, *g)].y)+ 10)/20,5);
	}
	std::uniform_real_distribution<> dis(0,fulllength);

	edge_ti ei_startEdge;

	while (isforbidden)
	{
		auto randn = dis(gen);
		for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
			
			auto val = pow(((position[boost::source(*ei, *g)].y + position[boost::target(*ei, *g)].y) + 10) / 20, 5);
			if (randn < val) {
				ei_startEdge = ei;
				break;
			}
			randn -= val;
		}


		isforbidden = forbiddenPm[*ei_startEdge] && CHECKFORBIDDEN;
		//isforbidden = false;
	}
	return ei_startEdge;
}




edge_ti getRandomEdgeWeighted(Graph* g) {
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	int iteratorLength = 0;
	int randiter;
	bool isforbidden = true;
	float fulllength = 0, tensionlength = 0, voxellength=0;


	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		iteratorLength++;

		tensionlength += restLengthPm[*ei] / currentLengthPm[*ei];
		fulllength += currentLengthPm[*ei];
	}

	std::uniform_real_distribution<> dis(0.0, 1.0);

	edge_ti ei_startEdge;

	while (isforbidden)
	{
		auto randn = dis(gen);
		for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
			if (randn < restLengthPm[*ei] / currentLengthPm[*ei]) {
				ei_startEdge = ei;
				break;
			}
			randn -= restLengthPm[*ei] / currentLengthPm[*ei]/tensionlength/2;
			randn -= currentLengthPm[*ei] / fulllength/2;


		}
		isforbidden = forbiddenPm[*ei_startEdge] && CHECKFORBIDDEN;
		//isforbidden = false;
	}


	return ei_startEdge;
}



edge_ti getRandomEdge(Graph* g) {
	tie(ei, eiend) = boost::edges(*g);
	int iteratorLength = 0;
	int randiter;
	bool isforbidden = true;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei)
		iteratorLength++;

	auto ei_startEdge = boost::edges(*g).first;
	
	while (isforbidden)
	{
		ei_startEdge = boost::edges(*g).first;
		randiter = rand() % iteratorLength;

		for (size_t i = 0; i < randiter; i++)
		{
			ei_startEdge++;
		}

		isforbidden=forbiddenPm[*ei_startEdge] && CHECKFORBIDDEN;
		//isforbidden = false;
	}
	

	return ei_startEdge;
}


edge_t getRandomEdgeFromEdgeList(Graph* g, std::vector<edge_t> edges, std::vector<size_t> edgeindices, size_t& cycleind) {
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	int iteratorLength = 0;
	int randiter;
	float fulllength = 0, tensionlength = 0, voxellength = 0;
	edge_t ed_resultedge;
	std::uniform_real_distribution<> dis(0.0, 1.0);
	// calculate max values:
	{
		for (auto ei = edges.begin(); ei != edges.end(); ++ei) {
			iteratorLength++;
			tensionlength += restLengthPm[*ei] / currentLengthPm[*ei];
			fulllength += currentLengthPm[*ei];
		}
	}
	auto randn = dis(gen);
	size_t countr = 0;
	for (auto ei = edges.begin(); ei != edges.end(); ++ei) {
		if (randn < restLengthPm[*ei] / currentLengthPm[*ei]/tensionlength) {
			ed_resultedge = *ei;
			cycleind = edgeindices[countr];
			break;
		}
		randn -= restLengthPm[*ei] / currentLengthPm[*ei] / tensionlength;
		//randn -= currentLengthPm[*ei] / fulllength / 2;
		countr++;
	}

	return ed_resultedge;
}



template <typename T>

edge_t getRandomEdgeFromEdgeListT(Graph* g, T begin, T end, bool forbcheck = false) {
	T cachebegin = begin;
	T cacheend = end;

	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	int iteratorLength = 0;
	int randiter;
	float fulllength = 0, tensionlength = 0, voxellength = 0;
	bool isforbidden = true;

	edge_t ed_resultedge;

	// calculate max values:
	{
		for (auto iter = begin; iter != end; ++iter) {
			iteratorLength++;
			fulllength += voxelMap[{(int)(position[boost::source(*iter, *g)].x),
				(int)(position[boost::source(*iter, *g)].y),
				(int)(position[boost::source(*iter, *g)].z)}];

			//fulllength += pow(((position[boost::source(*iter, *g)].y + position[boost::target(*iter, *g)].y) + 10) / 20, 1);

		}
	}
	std::uniform_real_distribution<> dis(0.0, fulllength);

	while (isforbidden)
	{
		begin = cachebegin;
		auto randn = dis(gen);
		size_t countr = 0;
		for (auto iter = begin; iter != end; ++iter) {
			//auto val = pow(((position[boost::source(*iter, *g)].y + position[boost::target(*iter, *g)].y) + 10) / 20, 1);
			auto val = voxelMap[{(int)(position[boost::source(*iter, *g)].x),
				(int)(position[boost::source(*iter, *g)].y),
				(int)(position[boost::source(*iter, *g)].z)}];
			if (randn < val) {
				ed_resultedge = *iter;

				break;
			}
			randn -= val;
			//randn -= currentLengthPm[*ei] / fulllength / 2;
			countr++;
		}
		isforbidden = forbiddenPm[ed_resultedge] && CHECKFORBIDDEN && forbcheck;

	}
	return ed_resultedge;
}



vec3 interpolateBetweenPoints(vec3 point1, vec3 point2, float t) {
	return point1 + (point2 - point1) * t;
}

Color getColorFromInt(int val) {
	float r = sin(val)/2+.5;
	float g = (cos(val))/2+0.5;
	float b = sin(2* val) + cos(val) / 2/2+.5 ;

	//console() << "r " << r << "g  " << g << "b " << b << endl;

	return Color( r,g,b);
}

string to_string(vec3 vector) {
	return to_string(vector.x) + "," + to_string(vector.y) + "," + to_string(vector.z);
}

void exportGraph(Graph g) {
	ofstream myfile;
	myfile.open("positions.txt", std::ofstream::trunc);
	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {

		//myfile << "[" << "(" << to_string(position[boost::source(*ei, g)]) << ");(" << to_string(position[boost::target(*ei, g)]) << ")" << "]" << endl;
		if (!forbiddenPm[*ei]) {
			myfile << "[" << "{" << to_string(position[boost::source(*ei, g)]) << "};{" << to_string(position[boost::target(*ei, g)]) << "}" << "]" << endl;
		}
	}
	myfile.close();
}

Color cRamp(double ratio) {

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
	return Color(red/255, grn/255, blu/255);
}

void initVoxelMap(string filename) {
	ifstream MyReadFile(filename);
	string line;
	vector<vec3> result;
	vec3 cachevec = {};
	std::array<int, 3> key{ {0, 0, 0} };
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
				key[0] = (stof(token));
			}
			else if (counter == 1) {
				key[1] = stof(token);
			}
			else if (counter == 2) {
				key[2] = stof(token);
			}
			s.erase(0, pos + delimiter.length());
			counter++;
		}
		voxelMap[{key[0], key[1], key[2]}] = stof(s);
		result.push_back(cachevec);

	}
}