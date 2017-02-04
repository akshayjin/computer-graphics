#pragma once

#include <vector>
#include <string>

class DataHandler{
public:
	std::vector <float> xvalues;				//Grid coordinates
	std::vector <float> yvalues;
	std::vector <float> zvalues;
	std::vector <float> salinity;	
	
	int XAxis;
	int YAxis;
	int ZAxis;
	std::string filename;
	
	DataHandler(std::string fname){
		filename = fname;
	}
	
	void parseData();
	std::vector<std::string> split(const std::string &s, char delim);
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	bool isFloat( std::string myString );
	void stringClean(std::string& s);

};
