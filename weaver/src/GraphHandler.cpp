#include "GraphHandler.h"




void GraphHandler::setup() {
	nullVert = boost::graph_traits<Graph>::null_vertex();
}


void GraphHandler::initOriginalEdges() {

		evertex_ti evi, eviend;
		boost::graph_traits<EdgesGraph>::out_edge_iterator outei, outeind;
		ci::app::console() << "number of edges" << edgesG.m_vertices.size() << std::endl;
		edgesG.m_vertices.size();
		for (std::tie(evi, eviend) = boost::vertices(edgesG); evi != eviend; ++evi) {
			auto& uInd=edgesG[*evi].uniqueIndex;
			std::tie(outei, outeind) = boost::out_edges(*evi, edgesG);
			if (outei == outeind) {
				
				ci::app::console() << "adding edge number " << edgesG[*evi].uniqueIndex<< ", edge index" << edgeMap[edgesG[*evi].uniqueIndex].index << std::endl;
				originalEdges[edgeMap[edgesG[*evi].uniqueIndex].index].listOfEdges.push_back(edgeMap[edgesG[*evi].uniqueIndex]);
			}
			if (uInd == edgeMap[uInd].index) {
				originalEdges[edgeMap[uInd].index].startVert = edgeMap[uInd].sourceV;
				originalEdges[edgeMap[uInd].index].endVert = edgeMap[uInd].targetV;
			}
		}
		for (auto const& pair : originalEdges) {
			std::map<int, int> verts;
			for (auto const& edg : pair.second.listOfEdges) {
				verts[edg.sourceV]++;
				verts[edg.targetV]++;
				if (verts[edg.sourceV] == 2)
					verts.erase(edg.sourceV);
				if (verts[edg.targetV] == 2)
					verts.erase(edg.targetV);
			}

		}

		


		for (auto it = originalEdges.begin(); it != originalEdges.end(); it++)
		{			
			std::vector<EdgeContainer> newEdgeList;
			std::vector<int> VertexList;

			ci::app::console() << "--------------------"  << std::endl;
			ci::app::console() << "starting edgeVerts: " << it->second.printEdgeVerts() << std::endl;

			int newVertex;
			//it->second.findVertexInEdges
			std::vector<EdgeContainer> items;
			
			ci::app::console() << "startV: " << it->second.startVert << ", endV: " << it->second.endVert << std::endl;
			VertexList.push_back(it->second.startVert);
			
			items =it->second.findVertexInEdges(it->second.startVert);
			ci::app::console() << "items UI: " << items[0].uniqueIndex << std::endl;

			newEdgeList.push_back(items[0]);
			ci::app::console() << " NEL size: " << newEdgeList.size()<< std::endl;

			newVertex= (it->second.startVert == items[0].sourceV) ? items[0].targetV : items[0].sourceV;
			VertexList.push_back(newVertex);

			while (newVertex!=it->second.endVert) {
				ci::app::console() << "vertex being looed for:  " << newVertex << std::endl;
				items = it->second.findVertexInEdges(newVertex);
				ci::app::console() << "previous NEL UI" << newEdgeList.back().uniqueIndex << std::endl;

				if (items[0].uniqueIndex == newEdgeList.back().uniqueIndex)
					newEdgeList.push_back(items[1]);
				else if (items[1].uniqueIndex == newEdgeList.back().uniqueIndex) 
					newEdgeList.push_back(items[0]);
				ci::app::console() << "new NEL UI" << newEdgeList.back().uniqueIndex << std::endl;
				newVertex = (newVertex == newEdgeList.back().sourceV) ? newEdgeList.back().targetV : newEdgeList.back().sourceV;
				VertexList.push_back(newVertex);
				ci::app::console() << newVertex << std::endl;
			}
			std::vector<int > edgeInds;
			for (const auto& eitem : newEdgeList) {
				edgeInds.push_back(eitem.uniqueIndex);
			}
			it->second.listOfEdges = newEdgeList;
			it->second.listOfVertices = VertexList;
			ci::app::console() << "ending edge verts:" << it->second.printEdgeVerts() << std::endl;

		}


		
}

void GraphHandler::printOriginalEdges() {
	for (auto const& pair : originalEdges) {
	std:: stringstream text;
	text << "[" << pair.first << "]: " << " { ";
	for (auto const& edg : pair.second.listOfEdges) {
		text << edg.uniqueIndex << ", ";
	}

	text.seekp(-2, std::ios_base::end);
	text << " } , (";
	for (auto const& edg : pair.second.listOfVertices) {
		text << edg << ", ";
	}

	text.seekp(-2, std::ios_base::end);
	text << ") \n";

		data.my_log.AddLog(text.str().c_str());
	}
	

}


float GraphHandler::determinant(ci::vec3 a, ci::vec3 b, ci::vec3 c) {
	//https://en.wikipedia.org/wiki/Determinant
	//a.x a.y a.z
	//b.x b.y b.z
	//c.x c.y c.z
	return a.x * b.y * c.z + a.y * b.z * c.x + a.z * b.x * c.y - (a.z * b.y * c.x + a.y * b.x * c.z + a.x * b.z * c.y);
}

float GraphHandler::closestDist(ci::vec3 a0, ci::vec3 a1, ci::vec3 b0, ci::vec3 b1, ci::vec3 & firstpoint, ci::vec3 & secondpoint, bool clampAll, bool clampA0 , bool clampA1, bool clampB0, bool clampB1 ) {
	//https://stackoverflow.com/questions/2824478/shortest-distance-between-two-line-segments
	ci::vec3 A = a1 - a0;
	ci::vec3 B = b1 - b0;
	float magA = length(A);
	float magB = length(B);

	ci::vec3 _A = A / magA;
	ci::vec3 _B = B / magB;

	ci::vec3 _cross = cross(_A, _B);
	float denom = pow(length(_cross), 2);

	if (denom < 0.0001) {
		auto d0 = dot(_A, (b0 - a0));
		if (clampA0 || clampA1 || clampB0 || clampB1) {
			auto d1 = dot(_A, (b1 - a0));

			// Is segment B before A ?
			if (d0 <= 0 && 0 >= d1) { //                       CHECK IF THIS WORKS
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
			else if (d0 >= magA && magA <= d1) {//                       CHECK IF THIS WORKS
				if (clampA1 && clampB0) {
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
		if ((clampA0 && t0 < 0) || (clampA1 && t0 > magA)) {
			auto _dot = dot(_B, (pA - b0));
			if (clampB0 && _dot < 0)
				_dot = 0;
			else if (clampB1 && _dot > magB)
				_dot = magB;

			B = b0 + (_B * _dot);
		}
		// Clamp projection B
		if ((clampB0 && t1 < 0) || (clampB1 && t1 > magB)) {
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
	return length(pA - pB);
}

std::vector<RecipeContainer> GraphHandler::getRecipeInfo() {
	return recipeInfo;
}
void GraphHandler::AddAllVerts() {
	int currentsize = g.m_vertices.size();

	for (size_t i = 0; i < vertexInfo.size()-currentsize; i++)
	{
		boost::add_vertex(g);
	}

	for (int i = 0; i < vertexInfo.size(); i++) {
		g[i].index = vertexInfo[i].index;
		g[i].isfixed= vertexInfo[i].isfixed;
		if (g[i].isfixed) {
			g[i].pos = vertexInfo[i].pos;
		}
		
	}
}


void GraphHandler::computeCurrentLength() {
	vertex_t v1, v2;

	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, g);
		v2 = boost::target(*ei, g);
		g[*ei].currentlength = distance(g[v1].pos, g[v2].pos);
	}
}


void GraphHandler::addEdgeFromRecipe(int index) {
	// add new vertices -
	// connect new vertices to edge -
	// set length of new edge, taken from edges csv -
	// conncet new vertices with previus edges -
	// 
	// locate the indices of the edges that replace the divided edge.
	// locate them by theor indices.
	// 
	// 
	//
#if VERBOSITYLEVEL>0
	ci::app::console() << "---------------------------------------------" << std::endl;
	ci::app::console() << "adding edge from recipe step " << index << std::endl;
#endif 
	RecipeContainer& recipe = recipeInfo[index];
	EdgeContainer ENC = edgeMap[recipe.NewEdgeC];
	EdgeContainer ENA1 = edgeMap[recipe.NewEdgeA1];
	EdgeContainer ENA2 = edgeMap[recipe.NewEdgeA2];
	EdgeContainer ENB1 = edgeMap[recipe.NewEdgeB1];
	EdgeContainer ENB2 = edgeMap[recipe.NewEdgeB2];
	EdgeContainer EOA = edgeMap[recipe.OldEdgeA];
	EdgeContainer EOB = edgeMap[recipe.OldEdgeB];


	edge_t edgeC = weaverConnect(ENC.sourceV, ENC.targetV, ENC);
	if (ENA1.sourceV > ENA2.sourceV)
		std::swap(ENA1, ENA2);
	if (ENB1.sourceV > ENB2.sourceV)
		std::swap(ENB1, ENB2);
	edge_t edgeA1 = weaverConnect(EOA.sourceV,ENC.sourceV, ENA1);
	edge_t edgeA2 = weaverConnect(EOA.targetV, ENC.sourceV, ENA2);
	edge_t edgeB1 = weaverConnect(EOB.sourceV, ENC.targetV, ENB1);// INDEX PROBLEM
	edge_t edgeB2 = weaverConnect(EOB.targetV, ENC.targetV, ENB2);//INDEX PROBLEM
#if VERBOSITYLEVEL>2
	ci::app::console() << "removing edge with index" << EOA.uniqueIndex <<" by vertex " << EOA.sourceV << " and " << EOA.targetV << std::endl;
	ci::app::console() << "removing edge with index" << EOB.uniqueIndex << " by vertex " << EOB.sourceV << " and " << EOB.targetV << std::endl;
#endif
	boost::remove_edge(EOA.sourceV, EOA.targetV, g);
	boost::remove_edge(EOB.sourceV, EOB.targetV, g);
#if VERBOSITYLEVEL>2
	ci::app::console() << "removed edges"<< std::endl;
	
#endif
	computeCurrentLength();
#if VERBOSITYLEVEL>0
	data.my_log.AddLog("[info]  added edge, from recipe step %i.\n", index);
#endif
	//printoriginalEdges();
}

void GraphHandler::removeEdgeFromRecipe(int index) {
#if VERBOSITYLEVEL>0
	ci::app::console() << "---------------------------------------------"  << std::endl;
	ci::app::console() << "removing edge from recipe step " << index << std::endl;
#endif 
	RecipeContainer& recipe = recipeInfo[index];
	EdgeContainer ENC = edgeMap[recipe.NewEdgeC];
	EdgeContainer ENA1 = edgeMap[recipe.NewEdgeA1];
	EdgeContainer ENA2 = edgeMap[recipe.NewEdgeA2];
	EdgeContainer ENB1 = edgeMap[recipe.NewEdgeB1];
	EdgeContainer ENB2 = edgeMap[recipe.NewEdgeB2];
	EdgeContainer EOA = edgeMap[recipe.OldEdgeA];
	EdgeContainer EOB = edgeMap[recipe.OldEdgeB];

	boost::remove_edge(ENC.sourceV, ENC.targetV, g);
	boost::remove_edge(ENB1.sourceV, ENB1.targetV, g);
	boost::remove_edge(ENB2.sourceV, ENB2.targetV, g);
	boost::remove_edge(ENA1.sourceV, ENA1.targetV, g);
	boost::remove_edge(ENA2.sourceV, ENA2.targetV, g);
	//edge_t edgeOA = boost::add_edge(EOA.sourceV, EOA.targetV, g).first;
	//edge_t edgeOB = boost::add_edge(EOB.sourceV, EOB.targetV, g).first;
	weaverConnect(EOA.sourceV, EOA.targetV, EOA);
	weaverConnect(EOB.sourceV, EOB.targetV, EOB);
#if VERBOSITYLEVEL>2
	ci::app::console() << "removing edge by vertex " << ENC.sourceV << " and " << ENC.targetV << std::endl;
	ci::app::console() << "removing edge by vertex " << ENA1.sourceV << " and " << ENA1.targetV << std::endl;
	ci::app::console() << "removing edge by vertex " << ENA2.sourceV << " and " << ENA2.targetV << std::endl;
	ci::app::console() << "removing edge by vertex " << ENB1.sourceV << " and " << EOB.targetV << std::endl;
	ci::app::console() << "removing edge by vertex " << ENB2.sourceV << " and " << ENB2.targetV << std::endl;
	ci::app::console() << "removing edge by vertex " << ENA1.sourceV << " and " << ENA1.targetV << std::endl;
	ci::app::console() << "removing edge by vertex " << ENA2.sourceV << " and " << ENA2.targetV << std::endl;
#endif
	computeCurrentLength();
	//data.my_log.AddLog("[info]  added edge, from recipe step.");


}



void GraphHandler::updateEdge( edge_t edge_desc, EdgeContainer edge_cont) {
	g[edge_desc].restlength = edge_cont.restlength;
	g[edge_desc].uniqueIndex = edge_cont.uniqueIndex;
	g[edge_desc].isforbidden = edge_cont.isForbidden;
	g[edge_desc].index = edge_cont.index;


}


std::vector<EdgeContainer> GraphHandler::loadEdges(std::string fname) {
	std::vector<EdgeContainer> content;
	std::string row [6];
	std::string line, word;

	std::fstream file(fname, std::ios::in);
	int countA = 0;
	int countB = 0;
	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (countB != 0) {
				countA = 0;

				std::stringstream str(line);

				while (getline(str, word, ','))
					row[countA++] = (word);
				edgeMap[std::stoi(row[0])] = EdgeContainer{ std::stoi(row[0]),std::stoi(row[1]) ,std::stof(row[2]) ,std::stoi(row[3]) ,std::stoi(row[4]) ,std::stoi(row[5]) };
				content.push_back(EdgeContainer{ std::stoi(row[0]),std::stoi(row[1]) ,std::stof(row[2]) ,std::stoi(row[3]) ,std::stoi(row[4]) ,std::stoi(row[5]) });
				//ci::app::console() << row[0] << " ,  " << row[1] << " ,  " << row[2] << " ,  " << row[3] << " ,  " << row[4] << " ,  " << row[5] << std::endl;
#if VERBOSITYLEVEL>2
				ci::app::console() << row[0] << ": " << row[1] << " ,  " << row[2] << " ,  " << row[3] << " ,  " << row[4] << " ,  " << row[5]<< std::endl;
#endif 

			}
			
			countB++;
		}
	}
	edgeInfo = content;

	ci::app::console() << "added " << edgeInfo.size() << " edges to loadEdges" << std::endl;
	return content;
}


std::vector<RecipeContainer> GraphHandler::loadRecipe(std::string fname) {
	std::vector<RecipeContainer> content;
	std::string row[9];
	std::string line, word;
	
	std::fstream file(fname, std::ios::in);
	int countA = 0;
	int countB = 0;
	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (countB != 0) {
				countA = 0;

				std::stringstream str(line);

				while (getline(str, word, ','))
					row[countA++] = (word);
				content.push_back(RecipeContainer{ std::stoi(row[0]),std::stoi(row[1]) ,
													std::stoi(row[2]),std::stoi(row[3]) ,
													std::stoi(row[4]),std::stoi(row[5]) ,
													std::stoi(row[6]),
													std::stof(row[7]) ,std::stof(row[8]) });
#if VERBOSITYLEVEL>2
				ci::app::console() << row[0] << " , " << row[1] << " ,  " << row[2] << " ,  " << row[3] << " ,  " << row[4] << " ,  " << row[5] << " ,  " << row[6] << " ,  " << row[7] << " ,  " << row[8] << std::endl;
#endif 
			}

			countB++;
		}
	}
	recipeInfo = content;
	ci::app::console() << "added " << recipeInfo.size() << " recipe steps to loadRecipe" << std::endl;
	return content;
}

std::vector<VertexContainer> GraphHandler::loadVertices(std::string fname) {
	std::vector<VertexContainer> content;
	std::string row[5];
	std::string line, word;

	std::fstream file(fname, std::ios::in);
	int countA = 0;
	int countB = 0;
	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (countB != 0) {
				countA = 0;

				std::stringstream str(line);

				while (getline(str, word, ','))
					row[countA++] = (word);
				content.push_back(VertexContainer{ std::stoi(row[0]),row[1]=="1" ,{
													std::stof(row[2]),std::stof(row[3]) ,
													std::stof(row[4])} });
			}
			countB++;
		}
	}
	vertexInfo = content;
	ci::app::console() << "added " << recipeInfo.size() << " vertices to loadVertices" << std::endl;
	return content;
}

edge_t GraphHandler::connectAB(vertex_t endPointA, vertex_t endPointB, float rc, int ind, bool isforbidden) {
	float dd = distance(g[endPointA].pos, g[endPointB].pos);

	edge_t edge = boost::add_edge(endPointA, endPointB, g).first;
	g[edge].currentlength = dd;
	g[edge].restlength = dd * rc;
	g[edge].isforbidden = isforbidden;
	g[edge].index = ind;

	return edge;
}
edge_t GraphHandler::weaverConnect(vertex_t endPointA, vertex_t endPointB, EdgeContainer edgeInfo) {
	edge_t edge = boost::add_edge(endPointA, endPointB, g).first;
	g[edge].restlength = edgeInfo.restlength;
	g[edge].uniqueIndex = edgeInfo.uniqueIndex;
	g[edge].isforbidden = edgeInfo.isForbidden;
	g[edge].index = edgeInfo.index;
	ci::app::console() << "edgeInfo.uniqueindex" << edgeInfo.uniqueIndex << std::endl;
	ci::app::console() << "edgeInfo.index" << edgeInfo.index << std::endl;
	calculateDivEdgeLengths(edgeInfo.uniqueIndex);
#if VERBOSITYLEVEL>2
	ci::app::console() << "connecting edge by vertex " << endPointA << " and " << endPointB << ", giving it unique index: " << edgeInfo.uniqueIndex << std::endl;
#endif 
	return edge;
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





void GraphHandler::InitialWebFromObj(float rc, std::string filename) {
	bool hasCycle;
	std::ifstream MyReadFile(filename);
	std::string line;
	int numberOfVertices = 0;
	int numberOfEdges = 0;
	data.my_log.AddLog("[info] initializing starting graph.\n");
	ci::app::console() << "[info] initializing starting graph." << std::endl;
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


					ci::app::console() << "UI: " << edgeMap[numberOfEdges+1].uniqueIndex << " IF: " << stoi(vstrings[3]) << std::endl;
					edge_t e = weaverConnect(stoi(vstrings[1]) - 1, stoi(vstrings[2]) - 1, edgeMap[numberOfEdges+1]);
					//edge_t e = connectAB(stoi(vstrings[1]) - 1, stoi(vstrings[2]) - 1, rc);
					//g[e].isforbidden = stoi(vstrings[3]);
					////ci::app::console() << "UI: " << edgeMap[numberOfEdges].uniqueIndex << " IF: " << stoi(vstrings[3]) << std::endl;
//g[e].uniqueIndex = edgeMap[numberOfEdges].uniqueIndex;
//g[e].index = edgeMap[numberOfEdges].index;


numberOfEdges++;
				}
			}

		}


	}
	else {
	data.my_log.AddLog("[warning] Initial Graph not found. check in your directory.\n");
	}
	data.my_log.AddLog("added %i vertices \n", numberOfVertices);
	data.my_log.AddLog("added %i xxxx edges \n", numberOfEdges);
}
//
ci::vec3 GraphHandler::interpolate(ci::vec3 start, ci::vec3 end, float t) {
	return start + (end - start) * t;
}


void GraphHandler::relaxPhysics() {
	float k = 5.900f;

	float eps = 0.04f;
	Graph::vertex_descriptor v1, v2;
	for (std::tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, g);
		v2 = boost::target(*ei, g);

		if (length(g[v2].pos - g[v1].pos) > 0.000001 && g[*ei].restlength > 0.000001) {
			if (!g[v1].isfixed) {
				g[v1].movevec += 1 * k * (1 / g[*ei].restlength)*(g[*ei].currentlength - g[*ei].restlength) *
					normalize(g[v2].pos - g[v1].pos);
			}
			if (!g[v2].isfixed) {
				g[v2].movevec += 1 * k * (1 / g[*ei].restlength) * (g[*ei].currentlength - g[*ei].restlength) *
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

edge_t GraphHandler::getClosestEdgeFromRay(ci::Ray ray) {
	////https://www.youtube.com/watch?v=6vpXM1dskmY
	ci::vec3 edge_s, edge_e;
	edge_t closestEdge;
	ci::vec3 ray_s = ray.getOrigin();
	ci::vec3 ray_e = ray_s + ray.getDirection();
	float closestdist = FLT_MAX;
	float dist;
	ci::vec3 firstP;
	ci::vec3 secondP;
	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		edge_s = g[boost::source(*ei, g)].pos;
		edge_e = g[boost::target(*ei, g)].pos;
		dist = closestDist(edge_s, edge_e, ray_s, ray_e, firstP, secondP, false, true, true, false, false);
		//console() << dist << endl;
		if (dist < closestdist) {
			closestdist = dist;
			closestEdge = *ei;
		}
	}
	return closestEdge;
}

//void GraphHandler::printoriginalEdges() {
//	std::stringstream text;
//	for (auto const& pair : originalEdges) {
//		text << "{" << pair.first << ": ";
//		for (auto it = pair.second.begin(); it != pair.second.end(); it++) {
//			text << *it << " , ";
//		}
//		text << " " << "}\n";
//	}
//	ci::app::console() << text.str() << std::endl;
//
//}

void GraphHandler::setupEdgesGraph() {

	int edgeC_index;
	 boost::graph_traits<EdgesGraph>::vertex_descriptor KVertex;
	 int asdfasd = 0;
	for (auto edgeC = edgeInfo.begin();	edgeC != edgeInfo.end(); ++edgeC) {
		
		KVertex= boost::add_vertex(edgesG);
		edgesG[KVertex].uniqueIndex = edgeC->uniqueIndex;
		asdfasd++;
	}
	ci::app::console() << "added " << edgesG.m_vertices.size() << " vertices to edgesgraph" << std::endl;

	for (auto recipeC = recipeInfo.begin(); recipeC != recipeInfo.end(); recipeC++) {

		boost::add_edge(recipeC->OldEdgeA-1, recipeC->NewEdgeA1 - 1,edgesG);
		boost::add_edge(recipeC->OldEdgeA - 1, recipeC->NewEdgeA2 - 1, edgesG);
		boost::add_edge(recipeC->OldEdgeB - 1, recipeC->NewEdgeB1 - 1, edgesG);
		boost::add_edge(recipeC->OldEdgeB - 1, recipeC->NewEdgeB2 - 1, edgesG);
	}
	ci::app::console() << "added recipesteps to to edgesgraph" << std::endl;

}

void GraphHandler::calculateDivEdgeLengths(int edgeInt)
{
	//edgeInt -= 1;
	std::vector<float> DivEdgeLengths; 
	std::vector<float> DivEdgeLengthsInterp;
	std::vector<int> daughterEdges;
	ci::app::console() << "---------------------------" << std::endl;
	ci::app::console() << "edgeint:" << edgeInt<< std::endl;
	ci::app::console() << "UIND" << edgeMap[edgeInt].index << std::endl;
	auto edges = originalEdges[edgeMap[edgeInt].index].returnInbetweenEdges(edgeMap[edgeInt].sourceV, edgeMap[edgeInt].targetV);
	
	for (std::vector<EdgeContainer>::iterator ei = edges.begin(); ei != edges.end(); ++ei) {
		DivEdgeLengths.push_back(ei->restlength);
		DivEdgeLengthsInterp.push_back(ei->restlength);
		daughterEdges.push_back(ei->uniqueIndex);
	}

	float  sumOfAllLengths = std::accumulate(DivEdgeLengths.begin(), DivEdgeLengths.end(), (float)0);
	
	std::vector<float>::iterator de1, de2;
	de1 = DivEdgeLengthsInterp.begin();
	de2 = DivEdgeLengthsInterp.end();
	for (de1, de2; de1!= de2; ++de1) {
		*de1 = *de1 / sumOfAllLengths;
	}


	
	

	edgesG[edgeInt-1].daughterEdges = daughterEdges;
	edgesG[edgeInt-1].DivEdgeLengths = DivEdgeLengths;
	edgesG[edgeInt-1].interPts = DivEdgeLengthsInterp;
	ci::app::console() << "---------------------------" << std::endl;
}
