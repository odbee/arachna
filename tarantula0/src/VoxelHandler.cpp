#include "VoxelHandler.h"


void VoxelHandler::setup() {

}

void VoxelHandler::initVoxelMap(std::string filename) {
	// TODO: ADD MIN BOUNDS AND MAX BOUNDS
	std::ifstream MyReadFile(filename);
	std::string line;
	std::vector<ci::vec3> result;
	ci::vec3 cachevec = {};
	std::array<int, 3> key{ {0, 0, 0} };
	size_t counter = 0;
	data.my_log.AddLog("[info] initializing voxelmap.\n");
	if (MyReadFile.good()) {

		while (std::getline(MyReadFile, line)) {
			counter = 0;
			std::string s = line;
			std::string delimiter = ",";
			size_t pos = 0;
			std::string token;
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);
				if (counter == 0) {
					key[0] = (stof(token));
				}
				else if (counter == 1) {
					key[1] = stof(token);
				}
				else if (counter == 2) {
					key[2] = stof(token);
				}
				s.erase(0, pos + delimiter.length());
				counter++;
			}
			voxels[{key[0], key[1], key[2]}] = stof(s);
			result.push_back(cachevec);

		}
	}
	else {
		data.my_log.AddLog("[warning] Initial Graph not found. check in your directory.\n");

	}
}
