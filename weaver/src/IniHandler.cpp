#include "IniHandler.h"

void IniHandler::setup() {
	filename = "cacheddata.ini";
	initImGui(filename);
}
void IniHandler::makeStartIni() {
	std::ofstream outfile(filename);
	outfile << "";
	outfile.close();
	//Add new key value pairs
	pt.add("graph directory", "dir");
	dirPath = "dir";
	pt.add("communication port", "COM0");
	comPort = "COM0";
	boost::property_tree::ini_parser::write_ini(filename, pt);
}

void IniHandler::initImGui(const std::string& inputfile) {
	filename = inputfile;

	std::ifstream MyReadFile(filename);
	if (!MyReadFile.good()) {
		makeStartIni();
	}
	else {
		boost::property_tree::ini_parser::read_ini(filename, pt);
		dirPath = pt.get<std::string>("graph directory");
		comPort = pt.get<std::string>("communication port");
		
	}

	//std::cout << pt.get<std::string>("Section1.Value1") << std::endl;
	//std::cout << pt.get<std::string>("Section1.Value2") << std::endl;
}

void IniHandler::writeImGui() {
	boost::property_tree::ini_parser::write_ini(filename, pt);
}