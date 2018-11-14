#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

class DataProcess {

private:
	std::string tempFileName;

public:
	DataProcess ();
	DataProcess (std::string name);
	~DataProcess ();

	std::string getDateTimeNow ();
	void appendData (std::string data);
};