#pragma once
#include "GraphSetup.h"
#include "HelperFunctions.h"
#include "CycleImportFunctions.h"

void resetweb(Graph& g,float relaxc, std::vector<std::vector<size_t>>& cycles) {
	g.clear();
	cycles.clear();
	voxelMap.clear();
	initVoxelMap(VOXELFILENAME);
	InitialWebFromObj(&g, relaxc, OBJFILENAME, cycles);
	if (CYCLESFILENAME!="") {
		loadcycles(&g, cycles, CYCLESFILENAME);
		addCyclesToVertices(&g, cycles);

	}
	else {
		findAndAddCycles(&g, cycles);
	}
	//InitialWebFromPc(&g, relaxc, VERTEXFILENAME);
	iterationcounter = 0;
}