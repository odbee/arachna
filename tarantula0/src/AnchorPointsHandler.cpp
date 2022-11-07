#include "AnchorPointsHandler.h"





void AnchorPointsHandler::setup() {

}

void AnchorPointsHandler::initAnchorPoints(std::string filename) {

	std::ifstream MyReadFile(filename);
	std::string line;
	std::vector<ci::vec3> result;
	ci::vec3 cachevec = {};
	std::array<int, 3> key{ {0, 0, 0} };
	size_t counter = 0;
	while (getline(MyReadFile, line)) {
		counter = 0;
		std::string s = line;
		std::string delimiter = ",";
		size_t pos = 0;
		std::string token;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			if (counter == 0) {
				cachevec.x = (stof(token));
			}
			else if (counter == 1) {
				cachevec.y = (stof(token));
			}
			s.erase(0, pos + delimiter.length());
			counter++;
		}

		cachevec.z = stof(s);
		anchorPoints.push_back(cachevec);

		ci::app::console() << "added point at" << cachevec.x << ", " << cachevec.y << ", " << cachevec.z << std::endl;
	}
}

