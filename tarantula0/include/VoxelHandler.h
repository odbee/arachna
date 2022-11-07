#pragma once
#include "cinder/app/App.h"

#include <vector>
#include <string>
#include <fstream>
#include <array>

class VoxelHandler
{
	private:
		void setup();
	public:
		VoxelHandler() { setup(); }
		std::map<std::array<signed int, 3>, float> voxelMap;
		bool hasVoxelMap = false;
		ci::vec3 boundsMin;
		ci::vec3 boundsMax;

		void initVoxelMap(std::string filename);
};

