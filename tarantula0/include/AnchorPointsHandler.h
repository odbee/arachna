#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include "DataContainer.h"

#include "cinder/app/App.h"
#include "GraphSetup.h"

class AnchorPointsHandler {
private:
	void setup();
public:

	AnchorPointsHandler(DataContainer& DATA) : data(DATA) { setup(); }
	DataContainer& data;

	anchor_points& anchorPoints=data.anchorPoints;
	bool hasAnchorPoints = false;
	void initAnchorPoints(std::string filename);
};
