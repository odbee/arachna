#pragma once
#include "cinder/app/App.h"
#include "GraphSetup.h"
#include "DataContainer.h"
#include <vector>
#include <string>
#include <fstream>
#include <array>

class VoxelHandler
{
	private:
		void setup();
	public:
		VoxelHandler(DataContainer& DATA) : data(DATA) { setup(); }
		DataContainer& data;
		voxel_map& voxels=data.voxels;
		bool hasVoxelMap = false;
		ci::vec3 boundsMin;
		ci::vec3 boundsMax;

		void initVoxelMap(std::string filename);
};

