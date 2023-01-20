#pragma once

#include "serialib.h"
#include "DataContainer.h"
#include "GraphHandler.h"
#include "serialib.h"
class ArduPort {
private:
	
	void setup();
public:
	ArduPort(DataContainer& DATA,GraphHandler& GH) : data(DATA), gh(GH) { setup(); }
	GraphHandler& gh;
	DataContainer& data;
	std::string& dirPath = data.fullPath;
	std::string& comPort = data.COMPort;
	serialib arduino;
	std::string connectArduino(std::string port, unsigned int baudrate = 115200U);
	int checkForInit(int timeout = 10);
	void printEdge(int edgeIndex);
	int sendCommandAndWait(std::string command, int timeout=10);
};
