#include "GraphHandler.h"


void GraphHandler::setup() {
	nullVert = boost::graph_traits<Graph>::null_vertex();
}

void GraphHandler::addCyclesToVertices() {
	for (int i = 0; i < cycles.size(); i++)
	{
		for (const auto& elem : cycles[i])
			g[elem].cycles.push_back(i);
	}
}
edge_t GraphHandler::connectAB(vertex_t endPointA, vertex_t endPointB, float rc, int ind, bool isforbidden) {
	float dd = distance(g[endPointA].pos, g[endPointB].pos);

	edge_t edge = boost::add_edge(endPointA, endPointB, g).first;
	g[edge].currentlength = dd;
	g[edge].restlength = dd * rc;
	g[edge].isforbidden = isforbidden;

	if (ind == 0) {
		g[edge].index = ++GLOBALINT;
	}
	else {
		g[edge].index = ind;
	}
	return edge;
}

void GraphHandler::emptyCyclesFromVertices() {
	for (std::tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {
		g[*vi].cycles.clear();
	}
}

void GraphHandler::exportGraph(std::string directory) {
	std::ofstream myfile;
	myfile.open("positions.txt", std::ofstream::trunc);
	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {

		//myfile << "[" << "(" << to_string(position[boost::source(*ei, g)]) << ");(" << to_string(position[boost::target(*ei, g)]) << ")" << "]" << endl;
		if (!g[*ei].isforbidden) {
			myfile << "[" << "{" << std::to_string(g[boost::source(*ei, g)].pos) << "};{" << std::to_string(g[boost::target(*ei, g)].pos) << "}" << "]" << std::endl;
		}
	}
	myfile.close();
}

void GraphHandler::exportGraphOBJ(Graph g, std::string directory) {
	std::ofstream myfile;
	myfile.open(directory, std::ofstream::trunc);

	for (std::tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {
		//gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
		myfile << "v " << std::to_string(g[*vi].pos.x) << " " << std::to_string(g[*vi].pos.y) << " " << std::to_string(g[*vi].pos.z) << std::endl;

	}

	for (std::tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {

		//myfile << "[" << "(" << to_string(position[boost::source(*ei, g)]) << ");(" << 
		if (!g[*ei].isforbidden) {
			myfile << "l " << std::to_string(boost::source(*ei, g) + 1) << " " << std::to_string(boost::target(*ei, g) + 1) << std::endl;
		}
	}
	myfile.close();
}

float GraphHandler::getDivPoint(edge_t edge) {
	std::vector<std::array<float, 2>> paramlist = g[edge].densityparams;
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<> dis(0.0, 1.0);
	auto randn = dis(gen);
	float result = 0.5;
	float val = 0;
	int paramslength = paramlist.size();

	for (size_t i = 0; i < paramslength; i++)
	{
		auto param = (g[edge].densityparams[i]);
		auto paramplus = (g[edge].densityparams)[(i + 1) % paramslength];
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

std::optional<edge_t> GraphHandler::GetEdgeFromItsVerts(vertex_t v, vertex_t u, Graph const& g)
{
	for (auto e : boost::make_iterator_range(out_edges(v, g))) {
		if (target(e, g) == u)
			return e;
	}
	return {};
	throw std::domain_error("my_find_edge: not found");
}


template <typename T>

edge_t GraphHandler::getRandomEdgeFromEdgeListIntegrated(T& begin, T& end, bool forbcheck, float density, float length,float tension) {
	T cachebegin = begin;
	T cacheend = end;
	//ci::app::console() << "getting random edge:";

	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	int iteratorLength = 0;	int randiter;
	float fulllength = 0, tensionlength = 0, voxellength = 0;
	bool isforbidden = true;

	edge_t ed_resultedge = *begin; // ADD FIRST EDGE TO RESULTEDGE SO IF AN ERROR OCCURS FALL BACK TO FIRST EDGE

	// calculate max values:
	{
		for (T iter = begin; iter != end; ++iter) {

			integrateEdge(*iter,voxelMap);
			g[*iter].probability;
			g[*iter].currentlength;
			g[*iter].restlength;
			voxellength += g[*iter].probability;
			fulllength += g[*iter].currentlength;
			tensionlength += g[*iter].restlength / g[*iter].currentlength;
			//ci::app::console() << g[*iter].isforbidden;
		}
	}
	//ci::app::console() << std::endl;
	std::uniform_real_distribution<> dis(0.0, 1.0);

	while (isforbidden)
	{
		begin = cachebegin;
		auto randn = dis(gen);
		size_t countr = 0;
		for (auto iter = begin; iter != end; ++iter) {

			auto voxelval = g[*iter].probability * density / voxellength;
			//auto voxelval = 0;
			auto lengthval = g[*iter].currentlength * length / fulllength;
			auto tensionval = (g[*iter].restlength / g[*iter].currentlength) * tension / tensionlength;

			auto val = voxelval + lengthval + tensionval;
			if (randn < val) {
				ed_resultedge = *iter;

				break;
			}
			randn -= val;
			//randn -= currentLengthPm[*ei] / fulllength / 2;
			countr++;
		}
		isforbidden = g[ed_resultedge].isforbidden && forbcheck;
		//ci::app::console() << "forbidden" << std::endl;
	}
	return ed_resultedge;
}


float GraphHandler::integrateEdge(edge_t edge, voxel_map voxelMap) {
	ci::vec3 p_start = g[boost::source(edge, g)].pos;
	ci::vec3 p_end = g[boost::target(edge, g)].pos;
	ci::vec3 dir = p_end - p_start;

	float param = 0;
	std::vector<std::array<float, 2>> params;


	coordcont xcont = { p_start.x,p_end.x, {1,0,0} };	xcont.calcvals();
	coordcont ycont = { p_start.y,p_end.y, {0,1,0} };	ycont.calcvals();
	coordcont zcont = { p_start.z,p_end.z, {0,0,1} };	zcont.calcvals();

	ci::vec3 currvox = { xcont.startvoxel,ycont.startvoxel,zcont.startvoxel };
	std::vector < coordcont > vecofconts = { xcont, ycont, zcont };
	std::sort(vecofconts.begin(), vecofconts.end());

	//checkIfInVoxelMap((int)xcont.startvoxel, (int)ycont.startvoxel, (int)zcont.startvoxel);

	float voxe = voxelMap[{ (int)xcont.startvoxel, (int)ycont.startvoxel, (int)zcont.startvoxel }];
	params.push_back({ 0.0f,voxe });

	for (int i = 0; i < std::max({ xcont.sign * xcont.direction, ycont.sign * ycont.direction, zcont.sign * zcont.direction }); i++)
	{
		for (auto s_voxel : vecofconts) {
			s_voxel.addparam(i, params, currvox, voxelMap);
		}

	}
	//console() << stringfromVec(params) <<endl;
	g[edge].densityparams = params;

	g[edge].probability = getProbablityFromParams(params, distance(p_start, p_end));
	//console() << probabilityPm[edge] << endl;
	return  g[edge].probability;

}







void GraphHandler::InitialWebFromObj(float rc, std::string filename) {
	bool hasCycle;
	std::ifstream MyReadFile(filename);
	std::string line;
	int numberOfVertices=0;
	data.my_log.AddLog("[info] initializing starting graph.\n");
	if (MyReadFile.good()) {
		while (getline(MyReadFile, line)) {
			std::stringstream ss(line);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> vstrings(begin, end);
			if (!vstrings.empty()) {
				if (vstrings[0] == "v") {
					auto a = boost::add_vertex(g);

					numberOfVertices++;
					g[a].pos = { stof(vstrings[1]),stof(vstrings[2]),stof(vstrings[3]) };
					if (vstrings.size() >= 5) {
						g[a].isfixed = stof(vstrings[4]); //
					}
					else {
						g[a].isfixed = true;
					}



				}
				if (vstrings[0] == "l") {
					edge_t e = connectAB(stoi(vstrings[1]) - 1, stoi(vstrings[2]) - 1, rc);
					g[e].isforbidden = stoi(vstrings[3]);
				}
			}

		}

		data.my_log.AddLog("added %i vertices \n", numberOfVertices);
	}
	else {
		data.my_log.AddLog("[warning] Initial Graph not found. check in your directory.\n");
	}

}


void GraphHandler::addRandomCyclicEdgeTesting(float rc) {
	cycles;
	// TODO FIX THE ISSUE WHEN THERE ARE NO AVAILABLE EDEGES
	cyclicedge startedge, goaledge;	edge_ti graphstart, graphend; edge_ti ei_startEdge; edge_t ed_startEdge;
	//getRandomEdgeWeightedByX(g);

	tie(ei, eiend) = boost::edges(g); //need this, will get error otherwise
	tie(graphstart, graphend) = boost::edges(g);

	ed_startEdge = getRandomEdgeFromEdgeListIntegrated(graphstart, graphend, true, data.G_density, data.G_length, data.G_tension);
	initEdge(ed_startEdge, startedge);

	std::vector<size_t>edgeinds;

	auto connedges = getConnectedEdges(startedge, edgeinds, forbiddenCheck);

	int seconditer;	edge_t ed_goalEdge; edge_ti ei_goalEdge;
	if (connedges.size()) {



		ed_goalEdge = getRandomEdgeFromEdgeListIntegrated(connedges.begin(), connedges.end(), forbiddenCheck,data.G_density,data.G_length,data.G_tension); // also accept forbidden edges


		size_t cycleIndex = *(edgeinds.begin() + std::distance(connedges.begin(), std::find(connedges.begin(), connedges.end(), ed_goalEdge)));

		initEdge(ed_goalEdge, goaledge);
		/*size_t cycleIndex = goaledge.cycles[0];*/
		std::vector<size_t> currCyc = cycles.at(cycleIndex);
		shiftCycle(currCyc, startedge.start.index, startedge.end.index);

		auto find1 = std::find(currCyc.begin(), currCyc.end(), goaledge.start.index);
		auto find2 = std::find(currCyc.begin(), currCyc.end(), goaledge.end.index);
		auto gei1 = std::distance(currCyc.begin(), find1);
		auto gei2 = std::distance(currCyc.begin(), find2);
		auto gei = (gei1 < gei2) ? find2 : find1;
		// get vetex indices


		std::vector<size_t> left(currCyc.begin(), gei);
		std::vector<size_t> right(gei, currCyc.end());
		startedge.divisionvert = boost::add_vertex(g); g[startedge.divisionvert].isfixed = false;
		goaledge.divisionvert = boost::add_vertex(g); g[goaledge.divisionvert].isfixed = false;
		//created 2 points
		//getDivPoint(startedge.descriptor);
		g[startedge.divisionvert].pos = interpolate(startedge, getDivPoint(startedge.descriptor));
		g[goaledge.divisionvert].pos = interpolate(goaledge, getDivPoint(goaledge.descriptor));
		//if (ah.hasAnchorPoints) {
		//	if (g[ed_startEdge].isforbidden) {
		//		ci::vec3 cp = getClosestPointFromList(g[goaledge.divisionvert].pos, ah.anchorPoints);
		//		ci::app::console() << "closest point distance" << distance(cp, g[goaledge.divisionvert].pos) << std::endl;
		//		g[startedge.divisionvert].pos = cp;
		//	}
		//	if (g[ed_goalEdge].isforbidden) {
		//		ci::vec3 cp = getClosestPointFromList(g[startedge.divisionvert].pos, ah.anchorPoints);
		//		ci::app::console() << "closest point distance" << distance(cp, g[startedge.divisionvert].pos) << std::endl;
		//		g[goaledge.divisionvert].pos = cp;
		//	}
		//}
		//position[startedge.divisionvert] = interpolate(startedge, float((float(rand() % 5) + 1) / 6));
		cycles.at(cycleIndex) = left;
		cycles.push_back(right);
		size_t lastindex = cycles.size() - 1;

		resolveIntersections(startedge, goaledge, cycleIndex, lastindex);

	}
	else {
		ci::app::console() << " no cycles found, returning" << std::endl;
		return;
	}
	connectEdges(startedge, goaledge, rc);

	//counter++;
	ci::app::console() << GLOBALINT << std::endl;
	updatetext("\n");
}


std::vector<edge_t> GraphHandler::getConnectedEdges(cyclicedge edge, std::vector<size_t>& edgeindices, bool forbCheck) {
	edge_t hs;
	std::vector<size_t> availverts;

	std::vector<edge_t> availedges;

	edge_t cachedge;
	int countr = 0;
	for (std::vector<size_t>& cycle : cycles) {
		std::vector<size_t>::iterator it1 = std::find(cycle.begin(), cycle.end(), edge.end.index);
		std::vector<size_t>::iterator it2 = std::find(cycle.begin(), cycle.end(), edge.start.index);
		if (it1 != cycle.end() && it2 != cycle.end()) {
			for (size_t i = 0; i < cycle.size(); i++)
			{
				// only add vertex if inside
				if (auto retedge = GetEdgeFromItsVerts(cycle[i], cycle[(i + 1) % cycle.size()], g)) {
					edge_t cachedge = *retedge;
					if (std::find(availedges.begin(), availedges.end(), cachedge) == availedges.end()) {
						// someName not in name, add it

						if ((!forbCheck || !g[cachedge].isforbidden) && cachedge != edge.descriptor) { //FORBIDDENCHECK ADDED
							availedges.push_back(cachedge);
							edgeindices.push_back(countr);
						}

						//if (cachedge != edge.descriptor) { //FORBIDDENCHECK ADDED
						//	availedges.push_back(cachedge);
						//	edgeindices.push_back(countr);
						//}
					}
				}


			}
		}
		countr++;
	}
	// TODO make clause for no edges, returning no edg

	return availedges;
}

std::vector<edge_t> GraphHandler::connectEdges(cyclicedge startedge, cyclicedge goaledge, float rc) {
	std::vector<edge_t> returnedges(5);

	returnedges[0] = connectAB(startedge.divisionvert, goaledge.divisionvert, rc);

	if (g[goaledge.descriptor].isforbidden) {
		returnedges[1] = connectAB(goaledge.divisionvert, goaledge.start.index, rc, g[goaledge.descriptor].index, true);
		returnedges[2] = connectAB(goaledge.divisionvert, goaledge.end.index, rc, g[goaledge.descriptor].index, true);
		g[goaledge.divisionvert].isfixed = true;
	}
	else {
		returnedges[1] = connectAB(goaledge.divisionvert, goaledge.start.index, rc, g[goaledge.descriptor].index);
		returnedges[2] = connectAB(goaledge.divisionvert, goaledge.end.index, rc, g[goaledge.descriptor].index);
	}

	if (g[startedge.descriptor].isforbidden) {
		returnedges[3] = connectAB(startedge.divisionvert, startedge.start.index, rc, g[startedge.descriptor].index, true);
		returnedges[4] = connectAB(startedge.divisionvert, startedge.end.index, rc, g[startedge.descriptor].index, true);
		g[startedge.divisionvert].isfixed = true;
	}
	else {
		returnedges[3] = connectAB(startedge.divisionvert, startedge.start.index, rc, g[startedge.descriptor].index);
		returnedges[4] = connectAB(startedge.divisionvert, startedge.end.index, rc, g[startedge.descriptor].index);
	}
	boost::remove_edge(startedge.descriptor, g);
	boost::remove_edge(goaledge.descriptor, g);
	return returnedges;
}

void GraphHandler::initEdge(edge_t ed, cyclicedge& cedge)
{
	cedge.descriptor = ed;
	cedge.cycles = compareVectorsReturnIntersection(g[source(cedge.descriptor, g)].cycles, g[boost::target(cedge.descriptor, g)].cycles);
	cedge.start = { source(cedge.descriptor, g), g[source(cedge.descriptor, g)].pos };
	cedge.end = { boost::target(cedge.descriptor, g), g[boost::target(cedge.descriptor, g)].pos };
}

ci::vec3 GraphHandler::interpolate(cyclicedge edge, float t) {
	return edge.start.pos + (edge.end.pos - edge.start.pos) * t;
}

void GraphHandler::removebyindex(std::vector<size_t>& vec, size_t ind) {
	vec.erase(std::remove(vec.begin(), vec.end(), ind), vec.end());
}

void GraphHandler::resolveIntersections(cyclicedge start, cyclicedge goal, size_t cycle1index, size_t cycle2index) {
	auto& left = cycles[cycle1index];
	auto& right = cycles[cycle2index];
	left.insert(left.end(), { goal.divisionvert, start.divisionvert });
	right.insert(right.begin(), { start.divisionvert, goal.divisionvert });
	for (const auto& elem : right)
		std::replace(g[elem].cycles.begin(), g[elem].cycles.end(), cycle1index, cycle2index);
	addIntersectionVertexToCycles(goal, cycle1index);
	addIntersectionVertexToCycles(start, cycle1index);
	auto commoncycles = compareVectorsReturnIntersection(goal.cycles, start.cycles);
	auto cright = vectorcycles(right);
	auto cleft = vectorcycles(left);


	for (const auto& cycleN : cright)
	{
		for (size_t i = 2; i < right.size(); i++)
		{
			auto elem = right[i];
			removebyindex(g[elem].cycles, cycleN);
			removebyindex(cycles[cycleN], elem);
		}
	}

	for (const auto& cycleN : cleft)
	{
		for (size_t i = 2; i < left.size(); i++)
		{
			auto elem = left[i];
			removebyindex(g[elem].cycles, cycleN);
			removebyindex(cycles.at(cycleN), elem);
		}
	}

	g[start.divisionvert].cycles.insert(g[start.divisionvert].cycles.end(), { cycle1index, cycle2index });
	g[goal.divisionvert].cycles.insert(g[goal.divisionvert].cycles.end(), { cycle1index, cycle2index });
}


std::vector<size_t> GraphHandler::vectorcycles(std::vector<size_t> vec) {
	std::vector<size_t> result = g[vec[0]].cycles;
	for (size_t i = 1; i < vec.size(); i++) {
		result = compareVectorsReturnIntersection(result, g[vec[i]].cycles);
	}
	return result;
}
void GraphHandler::updatetext(std::string in) {


	//gl::draw(mTextTexture);
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	std::ofstream myfile;
	myfile.open("example.txt", std::ios_base::app);
	myfile << in;
	myfile.close();
}

void GraphHandler::shiftCycle(std::vector<size_t>& cycle, int index1, int  index2) {
	auto edge1indInCycle_i = std::find(cycle.begin(), cycle.end(), index1);
	auto edge2indInCycle_i = std::find(cycle.begin(), cycle.end(), index2);
	auto cyclesize = cycle.size();
	if ((std::distance(cycle.begin(), edge1indInCycle_i) + 1) % cyclesize == std::distance(cycle.begin(), edge2indInCycle_i))       // if clockwise shift second vertex to first index
		rotate(cycle.begin(), edge2indInCycle_i, cycle.end());
	else if ((std::distance(cycle.begin(), edge2indInCycle_i) + 1) % cyclesize == std::distance(cycle.begin(), edge1indInCycle_i)) { // if counterclockwise flip list and do as in clockwise
		std::reverse(cycle.begin(), cycle.end());
		edge1indInCycle_i = std::find(cycle.begin(), cycle.end(), index1);
		edge2indInCycle_i = std::find(cycle.begin(), cycle.end(), index2);
		rotate(cycle.begin(), edge2indInCycle_i, cycle.end());
	}
	else {
		ci::app::console() << "vertex 1 and 2 are not adjacent" << std::endl;
	}
}



void GraphHandler::coordcont::calcvals() {
	start = start;
	end = end;
	direction = end - start;
	sign = sgn(direction);
	if (round(start + 0.5) != start + 0.5) {
		startvoxel = round(start);
	}
	else {
		startvoxel = (sign > 0) ? ceil(start) : floor(start);
	}
	if (round(end + 0.5) != end + 0.5) {
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
bool GraphHandler::coordcont::compareendvoxels() {
	return startvoxel != endvoxel;
}

bool GraphHandler::coordcont::sortbystartvoxeldist(const coordcont& x, const coordcont& y) {
	return x.startvoxeldist < y.startvoxeldist;
}


void GraphHandler::coordcont::addparam(int index, std::vector<std::array<float, 2>>& paramlist, ci::vec3& currentvoxel, voxel_map voxelMap) {
	if (compareendvoxels()) {
		if (index < sign * direction) {
			float param = (startvoxel + (index + 0.5) * sign - start) / direction;
			//checkIfInVoxelMap((int)currentvoxel.x, (int)currentvoxel.y, (int)currentvoxel.z);
			//*vh->at({ (int)currentvoxel.x, (int)currentvoxel.y, (int)currentvoxel.z });
			//*vh->voxelMap->at({ (int)currentvoxel.x, (int)currentvoxel.y, (int)currentvoxel.z });
			float voxe = voxelMap[{(int)currentvoxel.x, (int)currentvoxel.y, (int)currentvoxel.z}];
			if (param != 0 && param < 1) {

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
				currentvoxel += (float)sign * axis;
			}


		}
	}
}



void GraphHandler::addIntersectionVertexToCycles(cyclicedge& edge, int cycleIndex) {
	for (const auto& cycle : edge.cycles) {
		if (cycle != cycleIndex)
		{
			auto cyc = cycles.at(cycle);
			auto edge1indInCycle_i = std::find(cyc.begin(), cyc.end(), edge.start.index);
			auto edge2indInCycle_i = std::find(cyc.begin(), cyc.end(), edge.end.index);
			if ((std::distance(cyc.begin(), edge1indInCycle_i) + 1) % cyc.size() == std::distance(cyc.begin(), edge2indInCycle_i))       // if clockwise shift second vertex to first index
				cyc.insert(edge1indInCycle_i + 1, edge.divisionvert);
			else if ((std::distance(cyc.begin(), edge2indInCycle_i) + 1) % cyc.size() == std::distance(cyc.begin(), edge1indInCycle_i))       // if clockwise shift second vertex to first  // if counterclockwise shift first vertex to first index
				cyc.insert(edge2indInCycle_i + 1, edge.divisionvert);
			cycles.at(cycle) = cyc;
			g[edge.divisionvert].cycles.push_back(cycle);
		}
	}
}


void GraphHandler::relaxPhysics() {
	float k = 1.1f;
	float eps = 0.1f;
	Graph::vertex_descriptor v1, v2;
	for (std::tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, g);
		v2 = boost::target(*ei, g);
		
		if (length(g[v2].pos - g[v1].pos) > 0.01) {
			if (!g[v1].isfixed) {
				g[v1].movevec += 1 * k * (g[*ei].currentlength - g[*ei].restlength) *
					normalize(g[v2].pos - g[v1].pos);
			}
			if (!g[v2].isfixed) {
				g[v2].movevec += 1 * k * (g[*ei].currentlength - g[*ei].restlength) *
					normalize(g[v1].pos - g[v2].pos);
			}
		}
	}

	for (std::tie(vi, viend) = boost::vertices(g); vi != viend; ++vi) {

		//position[*vi] += moveVecPm[*vi];
		g[*vi].pos += eps * g[*vi].movevec;
		g[*vi].movevec = ci::vec3(0, 0, 0);
	}
	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, g);
		v2 = boost::target(*ei, g);
		g[*ei].currentlength = distance(g[v1].pos, g[v2].pos);
	}
}

edge_t GraphHandler::pickrandomEdge(edge_ti& begin, edge_ti& end) {
	edge_ti cachebegin = begin;
	edge_ti cacheend = end;
	ci::app::console() << "getting random edge:";

	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

	int iteratorLength = 0;	int randiter;
	float fulllength = 0, tensionlength = 0, voxellength = 0;
	bool isforbidden = true;

	edge_t ed_resultedge = *begin; // ADD FIRST EDGE TO RESULTEDGE SO IF AN ERROR OCCURS FALL BACK TO FIRST EDGE

	// calculate max values:
	{
		for (edge_ti iter = begin; iter != end; ++iter) {

			integrateEdge(*iter, voxelMap);
			g[*iter].probability;
			g[*iter].currentlength;
			g[*iter].restlength;
			voxellength += g[*iter].probability;
			fulllength += g[*iter].currentlength;
			tensionlength += g[*iter].restlength / g[*iter].currentlength;
			ci::app::console() << g[*iter].isforbidden;
		}
	}
	std::uniform_real_distribution<> dis(0.0, 1.0);

	begin = cachebegin;
	auto randn = dis(gen);
	ci::app::console() << randn << std::endl;
	size_t countr = 0;
	for (auto iter = begin; iter != end; ++iter) {

		auto voxelval = g[*iter].probability  / voxellength/3;
		//auto voxelval = 0;
		auto lengthval =( g[*iter].currentlength / fulllength)/3;
		auto tensionval = ((g[*iter].restlength / g[*iter].currentlength) / tensionlength)/3;

		auto val = voxelval + lengthval + tensionval;
		if (randn < val) {
			ed_resultedge = *iter;

			break;
		}
		randn -= val;
		//randn -= currentLengthPm[*ei] / fulllength / 2;
		countr++;
	}
	
	return ed_resultedge;
}
