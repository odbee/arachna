#include "GraphHandler.h"

void GraphHandler::setup() {
	nullVert = boost::graph_traits<Graph>::null_vertex();
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
	vertex_t v1, v2;
	RecipeContainer& recipe = recipeInfo[index];
	EdgeContainer ENC = edgeMap[recipe.NewEdgeC];
	EdgeContainer ENA1 = edgeMap[recipe.NewEdgeA1];
	EdgeContainer ENA2 = edgeMap[recipe.NewEdgeA2];
	EdgeContainer ENB1 = edgeMap[recipe.NewEdgeB1];
	EdgeContainer ENB2 = edgeMap[recipe.NewEdgeB2];
	EdgeContainer EOA = edgeMap[recipe.OldEdgeA];
	EdgeContainer EOB = edgeMap[recipe.OldEdgeB];

	//
	//g[ENC.sourceV].isfixed = false;
	//g[ENC.targetV].isfixed = false;
	//if (g[ENC.sourceV].isfixed)
	//	g[ENC.sourceV].pos = interpolate(g[EOA.sourceV].pos, g[EOA.targetV].pos,0.5);
	//
	ci::app::console() << "source fixed?" << g[ENC.sourceV].isfixed << std::endl;
	ci::app::console() << "target fixed?" << g[ENC.targetV].isfixed << std::endl;
	
	
	//if(g[ENC.targetV].isfixed)
	//	g[ENC.targetV].pos = interpolate(g[EOB.sourceV].pos, g[EOB.targetV].pos, 0.5);
	edge_t edgeC = boost::add_edge(ENC.sourceV, ENC.targetV, g).first;
	
	edge_t edgeA1 = boost::add_edge(ENC.sourceV, EOA.sourceV, g).first;
	edge_t edgeA2 = boost::add_edge(ENC.sourceV, EOA.targetV, g).first;
	edge_t edgeB1 = boost::add_edge(ENC.targetV, EOB.sourceV, g).first;
	edge_t edgeB2 = boost::add_edge(ENC.targetV, EOB.targetV, g).first;
	
	updateEdge(edgeC, ENC);
	updateEdge(edgeA1, ENA1);
	updateEdge(edgeA2, ENA2);
	updateEdge(edgeB1, ENB1);
	updateEdge(edgeB2, ENB2);


	boost::remove_edge(EOA.sourceV, EOA.targetV, g);
	boost::remove_edge(EOB.sourceV, EOB.targetV, g);
	ci::app::console() << "nwew edge at vertex " << ENC.sourceV << " and " << ENC.targetV << std::endl;

	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, g);
		v2 = boost::target(*ei, g);
		g[*ei].currentlength = distance(g[v1].pos, g[v2].pos);
	}
	data.my_log.AddLog("[info]  added edge, from recipe step %i.\n", data.INDEX);

}

void GraphHandler::removeEdgeFromRecipe(int index) {
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
	vertex_t v1, v2;
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
	edge_t edgeOA = boost::add_edge(EOA.sourceV, EOA.targetV, g).first;
	edge_t edgeOB = boost::add_edge(EOB.sourceV, EOB.targetV, g).first;

	updateEdge(edgeOA, EOA);
	updateEdge(edgeOB, EOB);
	ci::app::console() << "removing edge at vertex " << ENC.sourceV << " and " << ENC.targetV << std::endl;

	//boost::remove_vertex(ENC.sourceV, g);
	//boost::remove_vertex(ENC.targetV, g);

	for (tie(ei, eiend) = boost::edges(g); ei != eiend; ++ei) {
		v1 = boost::source(*ei, g);
		v2 = boost::target(*ei, g);
		g[*ei].currentlength = distance(g[v1].pos, g[v2].pos);
	}
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
			}
			
			countB++;
		}
	}
	edgeInfo = content;
	ci::app::console() << "added " << edgeInfo.size() << "edges" << std::endl;
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
			}

			countB++;
		}
	}
	recipeInfo = content;
	ci::app::console() << "added " << recipeInfo.size() << "recipe steps" << std::endl;
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
	ci::app::console() << "added " << recipeInfo.size() << "vertices" << std::endl;
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
					g[e].uniqueIndex = edgeMap[numberOfEdges].uniqueIndex;
					g[e].index = edgeMap[numberOfEdges].index;

					numberOfEdges++;
				}
			}

		}

		data.my_log.AddLog("added %i vertices \n", numberOfVertices);
	}
	else {
		data.my_log.AddLog("[warning] Initial Graph not found. check in your directory.\n");
	}

}
//
ci::vec3 GraphHandler::interpolate(ci::vec3 start, ci::vec3 end, float t) {
	return start+ (end - start) * t;
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