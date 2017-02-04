#pragma once

#include <vector>
#include <string>

class DataHandler{
public:
	std::vector <float> xvalues;				//Grid coordinates
	std::vector <float> yvalues;
	std::vector <std::vector<float>> salinity;	//Vector of salinity values indexed by timestamp
	
	int XAxis;
	int YAxis;
	int Timesteps;
	std::string filename;
	
	void parseData();
	std::vector<std::string> split(const std::string &s, char delim);
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	bool isFloat( std::string myString );
	void stringClean(std::string& s);

};
