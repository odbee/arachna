#pragma once
#include "GraphSetup.h"
#include "HelperFunctions.h"
#include "CycleImportFunctions.h"


void initWeb(Graph& g, float relaxc, std::vector<std::vector<size_t>>& cycles) {
	OBJFILENAME = dirPath + INITIALGRAPHDIRECTORYEXTENSION;
	CYCLESFILENAME = dirPath + CYCLESDIRECTORYEXTENSION;
	VOXELFILENAME = dirPath + VOXELSDIRECTORYEXTENSION;
	ANCHORPOINTSFILENAME = dirPath + ANCHORPOINTSDIRECTORYEXTENSION;
	anchorPoints.clear();
	initVoxelMap(VOXELFILENAME);
	initAnchorPoints(ANCHORPOINTSFILENAME);
	InitialWebFromObj(&g, relaxc, OBJFILENAME, cycles);
	loadcycles(&g, cycles, CYCLESFILENAME);
	addCyclesToVertices(&g, cycles);

}


void resetweb(Graph& g,float relaxc, std::vector<std::vector<size_t>>& cycles) {
	g.clear();
	cycles.clear();
	voxelMap.clear();
	anchorPoints.clear();
	initVoxelMap(VOXELFILENAME);
	initAnchorPoints(ANCHORPOINTSFILENAME);
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