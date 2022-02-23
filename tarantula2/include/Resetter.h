#pragma once

void resetweb(Graph& g,float relaxc, std::vector<std::vector<size_t>>& cycles) {
	g.clear();
	cycles.clear();
	InitialWebFromPc(&g, relaxc, VERTEXFILENAME);
	addcyclesfromPc(relaxc, g, cycles);
	iterationcounter = 0;
}