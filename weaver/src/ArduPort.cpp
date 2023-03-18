#include "ArduPort.h"

void ArduPort::setup() {
	arduino.closeDevice();
    arduino.flushReceiver();


}
std::string ArduPort::connectArduino(std::string port, unsigned int baudrate) {

	
    switch (arduino.openDevice(port.c_str(), baudrate)) {
        case 1:
            return "Successfully Connected to Device on Port \"" + port + "\" on baudrate " + std::to_string(baudrate);
        case -1:
            return "[error] connection unsuccessful, device not found";

        case -2:
            return "[error] connection unsuccessful, error while opening device";
        case -3:
            return "[error] connection unsuccessful, error while getting port parameters";
        case -4:
            return "[error] connection unsuccessful, Speed (Bauds) not recognized";
        case -5:
            return "[error] connection unsuccessful, error while writing port parameters";
        case -6:
            return "[error] connection unsuccessful, error while writing timeout parameters";
        case -7:
            return "[error] connection unsuccessful, Databits not recognized";
        case -8:
            return "[error] connection unsuccessful, Stopbits not recognized";
        case -9:
            return "[error] connection unsuccessful, Parity not recognized";
    }


}

int ArduPort::checkForInit(int timeout) {
    auto start = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point current;
    arduino.flushReceiver();

    while (true) {
        current = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = current - start;
        if (elapsed_seconds.count() > (double)timeout) {
            return 0;
            }

        if (arduino.available()) {
            char resultArray[128];
            auto hell = arduino.readString(resultArray, '\n', 128);

            if (hell) {
                data.my_log.AddLog("[ARDUINO]  %s\n", resultArray);
                std::string ret = resultArray;
                if (ret == "Starting Mark Mk3\n") {
                    sendCommandAndWait("D1 0");
                    sendCommandAndWait("D2 0");
                    return 1;
                }

            }
        }


    }
}


int ArduPort::sendCommandAndWait(std::string command, int timeout) {
    auto start = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point current;
    arduino.writeString(command.c_str());

    while (true) {
        current = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = current - start;
        if (elapsed_seconds.count() > (double)timeout) {
            return 0;
        }

        if (arduino.available()) {
            char resultArray[128];
            auto hell = arduino.readString(resultArray, '\n', 128);

            if (hell) {
                data.my_log.AddLog("[ARDUINO]  %s\n", resultArray);
                std::string ret = resultArray;
                if (ret == "movement successful\n") {
                    return 1;
                }

            }
        }


    }
}

void ArduPort::printEdge(int edgeIndex) {
    auto edges = gh.originalEdges[edgeIndex].listOfEdges;

    sendCommandAndWait("MB 50 MM", 3);
    sendCommandAndWait("D2 1", 3);
    sendCommandAndWait("D2 0", 3);

    for (const auto edg : edges) {
        edg.restlength;
        sendCommandAndWait("MB " + std::to_string(edg.restlength*10) +"  MM", 30);
        sendCommandAndWait("D1 1", 3);
        sendCommandAndWait("D1 0", 3);
    }
    sendCommandAndWait("D2 1", 3);
    sendCommandAndWait("D2 0", 3);
}
std::vector<int> ArduPort::split(const std::string& s, char delimiter) {
    std::vector<int> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(stoi(token));
    }
    return tokens;
}

void ArduPort::printEdgeFromList(std::string edgesString) {
    float rl;
    std::vector<int> newEdges_ints = split(edgesString, ',');

    if (newEdges_ints.size()) {
    std::vector<float> edgeLengths;
    sendCommandAndWait("MB 50 MM", 3);
    sendCommandAndWait("D2 1", 3);
    sendCommandAndWait("D2 0", 3);
    for (auto var : newEdges_ints)
    {
        rl = gh.edgeMap[var].restlength;
        sendCommandAndWait("MB " + std::to_string(rl     * 10) + "  MM", 30);
        sendCommandAndWait("D1 1", 3);
        sendCommandAndWait("D1 0", 3);
    }


    sendCommandAndWait("D2 1", 3);
    sendCommandAndWait("D2 0", 3);

    }


}

