#pragma once

#include <GraphSetup.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

class IniHandler {
	boost::property_tree::ptree pt;
	std::string filename;
	public:
		void makeStartIni() {

			//Add new key value pairs
			pt.add("graph directory", "dir");
			boost::property_tree::ini_parser::write_ini(filename, pt);
		}

		void initImGui(const std::string& inputfile) {
			filename = inputfile;

			std::ifstream MyReadFile(filename);
			if (!MyReadFile.good()) {
				makeStartIni();
			}
			else {
				boost::property_tree::ini_parser::read_ini(filename, pt);
				dirPath = pt.get<std::string>("graph directory");
			}

			//std::cout << pt.get<std::string>("Section1.Value1") << std::endl;
			//std::cout << pt.get<std::string>("Section1.Value2") << std::endl;
		}
		template<typename T>
		void overwriteTagImGui(std::string tag,T val) {
			pt.put(tag, val);
			boost::property_tree::ini_parser::write_ini(filename, pt);
			writeImGui();
		}
		void writeImGui() {
			boost::property_tree::ini_parser::write_ini(filename, pt);
		}

};





