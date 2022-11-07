#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include "cinder/app/App.h"

class AnchorPointsHandler {
private:
	void setup();
public:
	AnchorPointsHandler() { setup(); }
	std::vector<ci::vec3> anchorPoints;
	bool hasAnchorPoints = false;
	void initAnchorPoints(std::string filename);
};
