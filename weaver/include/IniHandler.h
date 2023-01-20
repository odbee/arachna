#pragma once

#include "DataContainer.h"
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
class IniHandler {
private:
	std::string filename;
	void setup();
public:
	boost::property_tree::ptree pt;

	IniHandler(DataContainer& DATA) : data(DATA) { setup(); }
	DataContainer& data;
	std::string& dirPath = data.fullPath;
	std::string& comPort = data.COMPort;

	void makeStartIni();

	void initImGui(const std::string& inputfile);
	template<typename T>
	void overwriteTagImGui(std::string tag, T val) {

		pt.put(tag, val);
		boost::property_tree::ini_parser::write_ini(filename, pt);
	}
	void writeImGui();

};


