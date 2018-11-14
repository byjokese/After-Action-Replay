#include "DataProcess.hpp"

//std::ofstream tempFile;
//Return the current DateTime String
std::string DataProcess::getDateTimeNow () {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	strftime (buffer, sizeof (buffer), "%d-%m-%Y_%H:%M:%S", timeinfo);
	std::string str (buffer);
	return str;
}

//Proccess Data with time and missionName
DataProcess::DataProcess (std::string name) {
	std::ostringstream missionStream;
	missionStream << name << "_" << getDateTimeNow () << ".json";
	tempFileName = missionStream.str ();
};
//Proccess Data only with time
DataProcess::DataProcess () {
	std::ostringstream missionStream;
	missionStream << getDateTimeNow () << ".json";;
	tempFileName = missionStream.str ();
}
//Deconstructor
DataProcess::~DataProcess () {
	//tempFile.close ();
}

void DataProcess::appendData (std::string data) {
	std::ofstream tempFile;
	tempFile.open (tempFileName, std::fstream::app);
	tempFile << data;
	tempFile.close ();
}
