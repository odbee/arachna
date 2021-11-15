#pragma once
#include "GraphSetup.h"

#include "HelperFunctions.h"

void drawPoints(Graph* g, mat4 proj, vec4 viewp, bool drawNumbers, bool drawVertexPoints, bool drawCycleList) {

	if (drawNumbers) {
		for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
			gl::drawLine(position[boost::source(*ei, *g)], position[boost::target(*ei, *g)]);
			vec3 danchorp = position[boost::source(*ei, *g)] + (position[boost::target(*ei, *g)] - position[boost::source(*ei, *g)]) * 0.5f;
			vec2 anchorp1 = glm::project(danchorp, mat4(), proj, viewp);
			gl::drawString(to_string(currentLengthPm[*ei] / restLengthPm[*ei]), anchorp1);
		}
	}
	if (drawVertexPoints)
	{
		for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {
			//gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
			vec2 anchorp1 = glm::project(position[*vi], mat4(), proj, viewp);
			gl::drawSolidCircle(anchorp1, 3);
			//console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
		}
	}

	if (drawCycleList)
	{
		for (tie(vi, viend) = boost::vertices(*g); vi != viend; ++vi) {

			gl::drawStrokedCube(position[*vi], vec3(0.2f, 0.2f, 0.2f));
			vec2 anchorp1 = glm::project(position[*vi], mat4(), proj, viewp);

			gl::drawString(to_string(*vi), anchorp1, Color::white(), Font("Times New Roman", 20));
			vec2 offset = vec2(0.0f, 22.0f);
			gl::drawString(stringfromCyclesShort(cyclesPm[*vi]), anchorp1 + offset, Color::hex(0xFF0000), Font("Times New Roman", 20));

			//console() << position[*vi].x << " , " << position[*vi].y << " , " << position[*vi].z << endl;
		}
	}
}

void drawGraph(Graph* g, mat4 proj, vec4 viewp, bool colorEdges= false) {
	gl::ScopedColor color(Color::gray(0.2f));
	gl::color(1.0f, 1.0f, 1.0f, 0.8f);
	for (tie(ei, eiend) = boost::edges(*g); ei != eiend; ++ei) {
		
		if (colorEdges){
			gl::color(getColorFromInt(indexPm[*ei]));
		}
		
		gl::drawLine(position[boost::source(*ei, *g)], position[boost::target(*ei, *g)]);

	}

}

void drawCycle(Graph* g, mat4 proj, vec4 viewp, std::vector<std::vector<size_t>> cycles,   int cycleNumber) {
	auto cycle = cycles[cycleNumber % (cycles.size())];
	gl::color(1.0f, 0.0f, 0.0f, 1.0f);
	for (const auto& i : cycle)
	{
		
		vec2 anchorp1 = glm::project(position[i], mat4(), proj, viewp);
		gl::drawSolidCircle(anchorp1, 5);
	}
	
}

void drawCycleEdges(Graph* g, mat4 proj, vec4 viewp, std::vector<std::vector<size_t>> cycles, int cycleNumber) {

	auto cycle = cycles[cycleNumber % (cycles.size())];
	gl::color(1.0f, 0.0f, 0.0f, 1.0f);
	int j;
	for (int i=0; i < cycle.size(); i++)	{
		j = (i + 1) % cycle.size();

		gl::drawLine(position[cycle[i]], position[cycle[j]]);
	}
}
