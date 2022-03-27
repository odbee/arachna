#pragma once
#include <GraphSetup.h>

using namespace ci;
using namespace ci::app;
using namespace std;

edge_t connectAB(Graph* g, Graph::vertex_descriptor endPointA, Graph::vertex_descriptor endPointB, float rc, int ind = 0, bool isforbidden = false) {
	float dd = distance(position[endPointA], position[endPointB]);

	edge_t edge = boost::add_edge(endPointA, endPointB, *g).first;
	currentLengthPm[edge] = dd;
	restLengthPm[edge] = dd * rc;
	forbiddenPm[edge] = isforbidden;
	return edge;
}


string stringfromCyclesShort(std::vector<size_t> cycleslist) {
	string outtext;
	for (const auto& elem : cycleslist) {
		outtext.append(to_string(elem) + "  ");

	}
	return outtext;
}

void addCyclesToVertices(Graph* g, std::vector<std::vector<size_t>>& cycles) {
	for (int i = 0; i < cycles.size(); i++)
	{
		for (const auto& elem : cycles[i])
			cyclesPm[elem].push_back(i);
	}
}

void addCycleToVertices(Graph * g, std::vector<size_t>cycle,int cycleInt) {
	for each (size_t var in cycle)
	{
		cyclesPm[var].push_back(cycleInt);
	}
}

void InitialWebFromObj(Graph* g, float rc, std::string filename, std::vector<std::vector<size_t>>& cycs) {
	bool hasCycle;
	std::ifstream MyReadFile(filename);
	std::string line;
	while (std::getline(MyReadFile, line)) {
		std::stringstream ss(line);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> vstrings(begin, end);
		if (!vstrings.empty()) {
			if (vstrings[0] == "v") {
				auto a = boost::add_vertex(*g);
				position[a] = { stof(vstrings[1]),stof(vstrings[2]),stof(vstrings[3]) };
				fixedBool[a] = true;
			}
			if (vstrings[0] == "l") {
				edge_t e = connectAB(g, stoi(vstrings[1]) - 1, stoi(vstrings[2]) - 1, rc);
				forbiddenPm[e] = stoi(vstrings[3]);
			}
		}

	}

	//boost::tiernan_all_cycles(g, vis);

	cycs = udgcd::findCycles<Graph, vertex_t>(*g);
	
	addCyclesToVertices(g, cycs);

}

void drawCycleEdges(Graph* g, mat4 proj, vec4 viewp, std::vector<std::vector<size_t>> cycles, int cycleNumber, Color col = Color(1.0f, 0.0f, 0.0f)) {

	auto cycle = cycles[cycleNumber % (cycles.size())];
	gl::color(col);
	int j;
	for (int i = 0; i < cycle.size(); i++) {
		j = (i + 1) % cycle.size();

		gl::drawLine(position[cycle[i]], position[cycle[j]]);
	}
}

void drawGraph(Graph* g, mat4 proj, vec4 viewp, Color primary =  { 1.0f, 1.0f, 1.0f }, Color secondary= ColorA(1.0f, 1.0f, 1.0f, 0.2f)) {
	gl::ScopedColor color(Color::gray(0.2f));
	gl::color(1.0f, 1.0f, 1.0f, 0.8f);
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		gl::color(1.0f, 1.0f, 1.0f);
		if (forbiddenPm[*ei]) {
			gl::color(secondary);
		}
		else {
			gl::color(primary);
		}
		gl::drawLine(position[boost::source(*ei, *g)], position[boost::target(*ei, *g)]);

	}

}

float getDistTwoEdges1(vec3 a1, vec3 a2, vec3 b1, vec3 b2) {
	return length((a2 - a1) * normalize(cross(b1, b2)));
}

float getDistTwoEdges(vec3 a1, vec3 a2, vec3 b1, vec3 b2) {
	vec3 zaehler = (a2 - a1) * cross(b1, b2); 
	float result = length(zaehler) * length(cross(b1, b2));
	return result;
}

float determinant(vec3 a, vec3 b, vec3 c) {
	//https://en.wikipedia.org/wiki/Determinant
	//a.x a.y a.z
	//b.x b.y b.z
	//c.x c.y c.z
	return a.x* b.y* c.z + a.y * b.z * c.x + a.z * b.x * c.y - (a.z * b.y * c.x + a.y * b.x * c.z + a.x * b.z * c.y);
}

float closestDist(vec3 a0, vec3 a1, vec3 b0, vec3 b1,vec3& firstpoint, vec3& secondpoint, bool clampAll = false, bool clampA0 = false, bool clampA1 = false, bool clampB0 = false,bool clampB1 = false) {
	//https://stackoverflow.com/questions/2824478/shortest-distance-between-two-line-segments
	vec3 A = a1 - a0;
	vec3 B = b1 - b0;
	float magA = length(A);
	float magB = length(B);

	vec3 _A = A / magA;
	vec3 _B = B / magB;

	vec3 _cross = cross(_A, _B);
	float denom = pow( length(_cross),2);

	if (denom < 0.0001) {
		auto d0 = dot(_A, (b0 - a0));
		if (clampA0 || clampA1 || clampB0 || clampB1) {
			auto d1 = dot(_A, (b1 - a0));

			// Is segment B before A ?
			if (d0 <= 0&&0 >= d1) { //                       CHECK IF THIS WORKS
				if (clampA0 && clampB1) {
					if (abs(d0) < abs(d1)) {
						firstpoint = a0;
						secondpoint = b0;
						return length(a0 - b0);
					}
						
				}
				firstpoint = a0;
				secondpoint = b1;
				return length(a0 - b1);
			}
			// Is segment B after A ?
			else if (d0 >= magA&& magA <= d1) {//                       CHECK IF THIS WORKS
				if (clampA1&&clampB0) {
					if (abs(d0) < abs(d1)) {
						firstpoint = a1;
						secondpoint = b0;
						return length(a1 - b0);
					}
					firstpoint = a1;
					secondpoint = b1;
					return length(a1 - b1);
				}
			}		
		}
		// Segments overlap, return distance between parallel segments
		firstpoint;
		secondpoint;
		return length(((d0 * _A) + a0) - b0);
	}
	auto t = (b0 - a0);
	auto detA = determinant(t, _B, _cross);
	auto detB = determinant(t, _A, _cross);
	auto t0 = detA / denom;
	auto t1 = detB / denom;
	auto pA = a0 + (_A * t0); // Projected closest point on segment A
	auto pB = b0 + (_B * t1); // Projected closest point on segment B
	// Clamp projections
	if (clampA0 || clampA1 || clampB0 || clampB1) {
		if (clampA0 && t0 < 0) 
			pA = a0;
		else if (clampA1 && t0 > magA)
			pA = a1;
		if (clampB0 && t1 < 0) 
			pB = b0;
		
		else if (clampB1 && t1 > magB)
			pB = b1;

		// Clamp projection A
		if ((clampA0 && t0 < 0) || (clampA1 &&t0 > magA)) {
			auto _dot = dot(_B, (pA - b0));
			if (clampB0&& _dot < 0)
				_dot = 0;
			else if (clampB1 && _dot > magB) 
				_dot = magB;
			
			B = b0 + (_B * _dot);
		}
		// Clamp projection B
		if ((clampB0 &&t1 < 0) || (clampB1 &&t1 > magB)){
			auto _dot = dot(_A, (pB - a0));
				if (clampA0 && _dot < 0) 
					_dot = 0;
				else if (clampA1 && _dot > magA) 
					_dot = magA;
			pA = a0 + (_A * _dot);
		}
	}

	firstpoint = pA;
	secondpoint = pB;
	return length(pA-pB);
}

float getDistLineLine(vec3 a1, vec3 a2, vec3 b1, vec3 b2) {
	vec3 u = a2 - a1;
	vec3 v = b2 - b1;
	vec3 w = a1 - b1;
	float a = dot(u, u);
	float b = dot(u, v);
	float c = dot(v, v);
	float d = dot(u, w);
	float e = dot(v, w);
	float D = a * c - b * b;
	float sc, tc;
	if (D < 0.000001) {
		sc = 0.0;
		tc = (b > c ? d / b : e / c);
	}
	else {
		sc = (b * e - c * d) / D;
		tc = (a * e - b * d) / D;
	}
	vec3 dP = w + (sc * u) - (tc * v);
	return length(dP);
}
void drawPoints(Graph* g, mat4 proj, vec4 viewp) {
	boost::graph_traits< Graph >::vertex_iterator vi, viend;

	for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {

		gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
		vec2 anchorp1 = glm::project(position[*vi], mat4(), proj, viewp);

		gl::drawString(to_string(*vi), anchorp1, Color::white(), Font("Arial", 20));

		vec2 offset = vec2(0.0f, 22.0f);
		gl::drawString(stringfromCyclesShort(cyclesPm[*vi]), anchorp1 + offset, Color::hex(0xFB4934), Font("Arial", 20));


		//console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
	}
}


edge_t getClosestEdgeFromRay(Ray ray, Graph *g) {
	////https://www.youtube.com/watch?v=6vpXM1dskmY
	vec3 edge_s, edge_e;
	edge_t closestEdge;
	vec3 ray_s = ray.getOrigin();
	vec3 ray_e =ray_s+ ray.getDirection();
	float closestdist = FLT_MAX;
	float dist;

	vec3 firstP;
	vec3 secondP;
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		edge_s= position[boost::source(*ei, *g)];
		edge_e= position[boost::target(*ei, *g)];
		dist = closestDist(edge_s, edge_e, ray_s, ray_e,firstP,secondP,false,true,true,false,false);
		//console() << dist << endl;
		if (dist < closestdist) {
			closestdist = dist;
			closestEdge = *ei;
		}
	}
	return closestEdge;
}

vector<size_t> compareVectorsReturnIntersection(const vector<size_t> vec1, const vector<size_t> vec2) {
	std::set<size_t> s1(vec1.begin(), vec1.end());
	std::set<size_t> s2(vec2.begin(), vec2.end());
	std::vector<size_t> v3;
	std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(v3));
	return v3;
}


void removeCycle(Graph* g, std::vector<std::vector<size_t>> &cycs, int cycleIndex) {

	cycs[cycleIndex].clear();
	boost::graph_traits< Graph >::vertex_iterator vi, viend;
	
	for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {
		if (std::find(cyclesPm[*vi].begin(), cyclesPm[*vi].end(), cycleIndex) != cyclesPm[*vi].end()) {
		cyclesPm[*vi].erase(std::find(cyclesPm[*vi].begin(), cyclesPm[*vi].end(), cycleIndex));
		}
	}
}
vector<int> split(const string& s, char delimiter) {
	vector<int> tokens;
	string token;
	istringstream tokenStream(s);
	while (getline(tokenStream, token, delimiter)) {
		tokens.push_back(stoi(token));
	}
	return tokens;
}


void removeActiveCycle(Graph* g, std::vector<std::vector<size_t>> cycs, int& currCycIndex, int& indinCyc, std::vector<size_t>& commoncyclelist) {
	int cachedIndex = currCycIndex;
	if (commoncyclelist.size() > 1) {
		indinCyc = (indinCyc + 1) % commoncyclelist.size();
		currCycIndex = commoncyclelist[indinCyc];
	}
	else {
		indinCyc = 0;
		currCycIndex = 0;
	}
	removeCycle(g, cycs, cachedIndex);

}

void addCycle(Graph* g, std::vector<std::vector<size_t>>& cycs, string cyclestring, int& currCycIndex, int& indinCyc, std::vector<size_t>& commoncyclelist) {
	vector<int> newcycle_ints = split(cyclestring,',');
	std::vector<size_t> newcycle(newcycle_ints.begin(), newcycle_ints.end());
	cycs.push_back(newcycle);
	addCycleToVertices(g, newcycle, cycs.size() - 1);

}