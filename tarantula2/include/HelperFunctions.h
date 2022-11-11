#pragma once
#include "GraphSetup.h"
#include <random>

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

template <typename T>
string stringfromVec(vector<T> vec) {
	string result = "[ ";
	for (auto it = vec.begin(); it != vec.end(); it++) {
		result.append(to_string(*it));
		result.append(" ");
	}
	result.append(" ]");
	return result;
}
void checkIfInVoxelMap(int x, int y, int z) {
	if (abs(x) > 5 && abs(y) > 5 && abs(z) > 5) {
		console() << "[ERROR] Trying to access voxel outside of range! Voxel number " << x << "," << y << "," << z << endl;
	}
}

float getProbablityFromParams(vector<std::array<float, 2>>& paramlist, float length) {
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

float getDivPoint(edge_t edge) {
	vector<std::array<float, 2>> paramlist = densityPm[edge];
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(0.0, 1.0);
	auto randn = dis(gen);
	float result = 0.5;
	float val= 0;
	int paramslength = paramlist.size();

	for (size_t i = 0; i < paramslength; i++)
	{
		auto param = (densityPm[edge])[i];
		auto paramplus = (densityPm[edge])[(i + 1) % paramslength];
		if (paramplus[0] < 0.001) {
			paramplus[0] = 1;
		}

		val = (paramplus[0] - param[0]) * param[1];
		if (randn < val) {
			result = param[0] + (paramplus[0] - param[0]) * 0.5f;
			break;
		}



	}
	return result;
}

string stringfromVec(vector<array<float,2>> vec) {
	string result = "[ ";
	for (auto it = vec.begin(); it != vec.end(); it++) {
		string str = to_string((*it)[0]) + "++" + to_string((*it)[1]) + "   ";
		result.append(str);
	}
	result.append(" ]");
	return result;
}


struct coordcont {
	float start;
	float end;
	vec3 axis;
	signed int sign;

	int startvoxel;
	int endvoxel;
	float startvoxeldist;
	float direction;
	void calcvals() {
		start = start;
		end = end;
		direction = end - start;
		sign = sgn(direction);
		if (round(start+0.5) != start+0.5) {
			startvoxel = round(start);
		} else {
			startvoxel = (sign > 0) ? ceil(start) : floor(start);
		}
		if (round(end+0.5) != end+0.5) {
			endvoxel = round(end);
		}
		else {
			endvoxel = (sign > 0) ? ceil(end) : floor(end);
		}
		if (round(start + 0.5) == start + 0.5) {
			startvoxeldist = (direction >= 1) ? 1 : direction;
		}

		else {
			startvoxeldist = (sign > 0) ? abs(start + 0.5 - ceil(start + 0.5)) : abs(start + 0.5 - floor(start + 0.5)); //offset by 0.5 to properly get floor/ceil
		}
		
		
		
		

	}
	bool compareendvoxels() {
		return startvoxel != endvoxel;
	}

	void addparam(int index, vector<std::array<float,2>>&paramlist,vec3& currentvoxel) {
		if (compareendvoxels()) {
			if (index < sign * direction) {
				float param = (startvoxel + (index+0.5) * sign - start) / direction;
				//checkIfInVoxelMap((int)currentvoxel.x, (int)currentvoxel.y, (int)currentvoxel.z);
				float voxe = voxelMap[{(int)currentvoxel.x, (int)currentvoxel.y, (int)currentvoxel.z}];
				if (param != 0&& param <1) {

					if (!paramlist.empty()) {
						if (paramlist.back()[0] != param) {

							paramlist.push_back({ param,voxe });
						}
						else if (paramlist.back()[0] == param) {
							paramlist.back()[1] = voxe;
						}
					}
					else {
						paramlist.push_back({ param,voxe });
					}
					currentvoxel += (float)sign*axis;
				}
				
				
			}
		}
	}
	
};
bool sortbystartvoxeldist(const coordcont& x, const coordcont& y) {
	return x.startvoxeldist < y.startvoxeldist;
}


float integrateEdge(edge_t edge, Graph& g) {
	vec3 p_start = position[boost::source(edge, g)];
	vec3 p_end = position[boost::target(edge, g)];
	vec3 dir = p_end - p_start;
	
	float param = 0;
	vector<std::array<float, 2>> params;


	coordcont xcont = { p_start.x,p_end.x, {1,0,0} };	xcont.calcvals();
	coordcont ycont = { p_start.y,p_end.y, {0,1,0} };	ycont.calcvals();
	coordcont zcont = { p_start.z,p_end.z, {0,0,1} };	zcont.calcvals();

	vec3 currvox = { xcont.startvoxel,ycont.startvoxel,zcont.startvoxel };
	vector < coordcont > vecofconts = { xcont, ycont, zcont};
	std::sort(vecofconts.begin(), vecofconts.end(),sortbystartvoxeldist);

	//checkIfInVoxelMap((int)xcont.startvoxel, (int)ycont.startvoxel, (int)zcont.startvoxel);
	float voxe = voxelMap[{(int)xcont.startvoxel , (int)ycont.startvoxel, (int)zcont.startvoxel}];
	params.push_back({ 0.0f,voxe });

	for (int i = 0; i < max({ xcont.sign * xcont.direction, ycont.sign * ycont.direction, zcont.sign * zcont.direction }); i++)
	{
		for (auto s_voxel : vecofconts) {
			s_voxel.addparam(i, params, currvox);
		}

	}
	//console() << stringfromVec(params) <<endl;
	densityPm[edge] = params;
	probabilityPm[edge] = getProbablityFromParams(params, distance(p_start, p_end));
	//console() << probabilityPm[edge] << endl;
	return probabilityPm[edge];

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


void addCyclesToVertices(Graph* g, std::vector<std::vector<size_t>>& cycles) {
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

template <typename T>

edge_t getRandomEdgeFromEdgeListT(Graph* g, T& begin, T& end, bool forbcheck = false) {
	T cachebegin = begin;
	T cacheend = end;

	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	int iteratorLength = 0;
	int randiter;
	float fulllength = 0, tensionlength = 0, voxellength = 0;
	bool isforbidden = true;

	edge_t ed_resultedge=*begin; // ADD FIRST EDGE TO RESULTEDGE SO IF AN ERROR OCCURS FALL BACK TO FIRST EDGE

	// calculate max values:
	{
		for (auto iter = begin; iter != end; ++iter) {
			iteratorLength++;
			array<signed int, 3> pos = {
				(signed int)(position[boost::source(*iter, *g)].x + 0.5 * (position[boost::target(*iter, *g)].x - position[boost::source(*iter, *g)].x)),
				(signed int)(position[boost::source(*iter, *g)].y + 0.5 * (position[boost::target(*iter, *g)].y - position[boost::source(*iter, *g)].y)),
					(signed int)(position[boost::source(*iter, *g)].z + 0.5 * (position[boost::target(*iter, *g)].z - position[boost::source(*iter, *g)].z))
					};

			voxellength += voxelMap[pos];
			fulllength+= currentLengthPm[*ei];
			tensionlength+= restLengthPm[*ei] / currentLengthPm[*ei];

			//fulllength += pow(((position[boost::source(*iter, *g)].y + position[boost::target(*iter, *g)].y) + 10) / 20, 1);

		}
	}
	std::uniform_real_distribution<> dis(0.0, 1.0);

	while (isforbidden)
	{
		begin = cachebegin;
		auto randn = dis(gen);
		size_t countr = 0;
		for (auto iter = begin; iter != end; ++iter) {
			array<signed int, 3> pos = {
				(signed int)(position[boost::source(*iter, *g)].x + 0.5 * (position[boost::target(*iter, *g)].x - position[boost::source(*iter, *g)].x)),
				(signed int)(position[boost::source(*iter, *g)].y + 0.5 * (position[boost::target(*iter, *g)].y - position[boost::source(*iter, *g)].y)),
				(signed int)(position[boost::source(*iter, *g)].z + 0.5 * (position[boost::target(*iter, *g)].z - position[boost::source(*iter, *g)].z))
			};
			auto voxelval= voxelMap[pos] *G_density/voxellength;
			auto lengthval= currentLengthPm[*ei]*G_length/fulllength;
			auto tensionval = (restLengthPm[*ei] / currentLengthPm[*ei]) * G_tension / tensionlength;

			auto val = voxelval + lengthval + tensionval;
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


template <typename T>

edge_t getRandomEdgeFromEdgeListIntegrated(Graph* g, T& begin, T& end, bool forbcheck = false) {
	T cachebegin = begin;
	T cacheend = end;

	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	int iteratorLength = 0;	int randiter;
	float fulllength = 0, tensionlength = 0, voxellength = 0;
	bool isforbidden = true;

	edge_t ed_resultedge = *begin; // ADD FIRST EDGE TO RESULTEDGE SO IF AN ERROR OCCURS FALL BACK TO FIRST EDGE

	// calculate max values:
	{
		for (auto iter = begin; iter != end; ++iter) {
			if (fixedBool[*iter]) {

			}	
			else {
				integrateEdge(*iter, *g);
				voxellength += probabilityPm[*iter];
				fulllength += currentLengthPm[*iter];
				tensionlength += restLengthPm[*iter] / currentLengthPm[*iter];
			}
		}
	}
	std::uniform_real_distribution<> dis(0.0, 1.0);

	while (isforbidden)
	{
		begin = cachebegin;
		auto randn = dis(gen);
		size_t countr = 0;
		for (auto iter = begin; iter != end; ++iter) {
			auto voxelval = probabilityPm[*iter] * G_density / voxellength;
			//auto voxelval = 0;
			auto lengthval = currentLengthPm[*iter] * G_length / fulllength;
			auto tensionval = (restLengthPm[*iter] / currentLengthPm[*iter]) * G_tension / tensionlength;

			auto val = voxelval + lengthval + tensionval;
			if (randn < val) {
				ed_resultedge = *iter;

				break;
			}
			randn -= val;
			//randn -= currentLengthPm[*ei] / fulllength / 2;
			countr++;
		}
		isforbidden = forbiddenPm[ed_resultedge] && forbcheck;
		console() << "forbidden" << endl;
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

void exportGraph(Graph g, string dir) {
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

void exportGraphOBJ(Graph g,string title="positions.obj") {
	ofstream myfile;
	myfile.open(title, std::ofstream::trunc);
	
	for (tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {
		//gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
		myfile << "v "<< to_string(position[*vi].x) << " " << to_string(position[*vi].y) << " " << to_string(position[*vi].z) << endl;
		
	}
	
	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {

		//myfile << "[" << "(" << to_string(position[boost::source(*ei, g)]) << ");(" << to_string(position[boost::target(*ei, g)]) << ")" << "]" << endl;
		if (!forbiddenPm[*ei]) {
			myfile << "l " << to_string(boost::source(*ei, g)+1) << " " << to_string(boost::target(*ei, g)+1)  << endl;
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

ImU32 cRampU32(double ratio) {

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
	return ImGui::ColorConvertFloat4ToU32({ red / 255, grn / 255, blu / 255,1.0f });
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


void initAnchorPoints(string filename) {
	
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
				cachevec.x = (stof(token));
			}
			else if (counter == 1) {
				cachevec.y = (stof(token));
			}
			s.erase(0, pos + delimiter.length());
			counter++;
		}

		cachevec.z = stof(s);
		anchorPoints.push_back(cachevec);
		console() << "added point at" << cachevec.x << ", " << cachevec.y << ", " << cachevec.z << endl;
	}
}





void adjustothers(vector<float*>& values, vector<float*>& cachedvalues,size_t index) {
	float factor=*cachedvalues[index]- *values[index];
	float sum = 0.0f;
	//factor /= values.size()-1;
	float multfactor;
	for (size_t i = 0; i < values.size(); i++)
	{
		if (i != index) {

			sum += *values[i];
		}
	}


	for (size_t i = 0; i < values.size(); i++)
	{
		if (i != index) {
			multfactor = (*values[i]) /sum;

			*values[i] += factor*multfactor;
		}
	}
	for (size_t i = 0; i < values.size(); i++) {
		const float uu = *values[i];
		*cachedvalues[i] = uu;
	}
}

void checkforchange(vector<float*>& values, vector<float*>& cachedvalues) {
	float  val, cachedval;
	//console() << "checking " << endl;
	for (size_t i = 0; i < values.size(); i++)
	{
		//console() << "checking " << endl;

		val = *values[i];
		cachedval= *cachedvalues[i];
		
		if (val!=cachedval)
		{
			adjustothers(values, cachedvalues, i);

		}
	}

}

void addcyclesfromPc(float relaxc,Graph&g, std::vector<std::vector<size_t>>&cycles){
	
	using FloatType = float;
	using vex3 = quickhull::Vector3<FloatType>;
	vector<vector<size_t>> convhull;
	quickhull::QuickHull<FloatType> qh;
	std::vector<vex3> pc;

	for (tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {

		pc.emplace_back(float(position[*vi][0]), float(position[*vi][1]), float(position[*vi][2]));

	}
	auto hull = qh.getConvexHull(&pc[0].x, pc.size(), true, true);
	auto indexbuffer = hull.getIndexBuffer();
	for (size_t i = 0; i < indexbuffer.size() / 3; i++)
	{
		convhull.push_back({ indexbuffer.begin() + 3 * i,indexbuffer.begin() + 3 * i + 3 });
	}

	for (const auto& cycl : convhull) {
		for (size_t i = 0; i < cycl.size(); i++)
		{
			auto v1 = cycl[i];
			auto v2 = cycl[(i + 1) % cycl.size()];
			//console() << v1 << "," << v2 << endl;
			typename boost::graph_traits<Graph>::adjacency_iterator ai, ai2, ai_end, ai_end2;
			boost::graph_traits< Graph >::vertex_iterator vi2, viend2;
			for (boost::tie(ai, ai_end) = boost::adjacent_vertices(v1, g),
				boost::tie(ai2, ai_end2) = boost::adjacent_vertices(v2, g);
				ai != ai_end && ai2 != ai_end2; ai++, ai2++) {
				if (*ai2 == *ai && v1 > v2) {
					//console() << " adjacancyts" << *ai << ',' << *ai2 << " ";
					cycles.push_back({ v1,v2,*ai });
				}
			}
		}
	}
	for (const auto& cycl : convhull) {
		for (size_t i = 0; i < cycl.size(); i++)
		{
			auto v1 = cycl[i];
			auto v2 = cycl[(i + 1) % cycl.size()];
			connectAB(&g, v1, v2, relaxc, 0, true);
		}
	}

	addCyclesToVertices(&g, cycles);

}

void findAndAddCycles(Graph* g, std::vector<std::vector<size_t>>& cycs) {
	cycs = udgcd::findCycles<Graph, vertex_t>(*g);
	addCyclesToVertices(g, cycs);
}

vec3 getClosestPointFromList(vec3 startPoint, std::vector<vec3> listOfPoints) {
	float mindist= std::numeric_limits<float>::max();
	vec3 cpPt;
	float compdist = 0;
	for (vec3 compPt : listOfPoints) {
		compdist =distance(startPoint, compPt);
		console()  << "dist : " << compdist << endl;
		if (compdist < mindist) {
			mindist = compdist;
			cpPt = compPt;
		}
	}
	return cpPt;
}