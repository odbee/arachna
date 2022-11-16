#pragma once
#include "GraphSetup.h"
#include "HelperFunctions.h"
#include "CycleImportFunctions.h"


void initWeb(Graph& g, float relaxc, std::vector<std::vector<size_t>>& cycles) {
	initVoxelMap(dirPath + VOXELSDIRECTORYEXTENSION);
	initAnchorPoints(dirPath + ANCHORPOINTSDIRECTORYEXTENSION);
	InitialWebFromObj(&g, relaxc, dirPath + INITIALGRAPHDIRECTORYEXTENSION, cycles);
	loadcycles(&g, cycles, dirPath + CYCLESDIRECTORYEXTENSION);
	addCyclesToVertices(&g, cycles);

}


void resetweb(Graph& g,float relaxc, std::vector<std::vector<size_t>>& cycles) {
	g.clear();
	cycles.clear();
	voxelMap.clear();
	anchorPoints.clear();
	iterationcounter = 0;
	initWeb(g, relaxc, cycles);

}