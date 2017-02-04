#include "include/DataHandler.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

void DataHandler::parseData()
{
ifstream in (filename);
	if (in.is_open())
	{
		string line;
		while ( getline (in,line) )
		{
		  stringClean(line);
		  if(line=="data:")
		  {
			getline (in,line);
			getline (in,line);
			stringClean(line);
			while(line!="")
			{
				vector<string> vals = split(line,',');
				for(int i=0;i<vals.size();i++)
				{
					if(isFloat(vals[i]))
					{
						xvalues.push_back(stof(vals[i]));
					}
				}
				getline (in,line);
				stringClean(line);
			}
			getline (in,line);
			stringClean(line);
			while(line!="")
			{
				vector<string> vals = split(line,',');
				for(int i=0;i<vals.size();i++)
				{
					if(isFloat(vals[i]))
					{
						yvalues.push_back(stof(vals[i]));
					}
				}
				getline (in,line);
				stringClean(line);
			}
			getline (in,line);
			stringClean(line);
			while(line!="")
			{
				vector<string> vals = split(line,',');
				for(int i=0;i<vals.size();i++)
				{
					if(isFloat(vals[i]))
					{
						zvalues.push_back(stof(vals[i]));
					}
				}
				getline (in,line);
				stringClean(line);
			}
			getline (in,line);
			stringClean(line);
		  }
		  if(line=="SAL,")
		  {
			  getline (in,line);
			  stringClean(line);
			  while(line!="}")
			  {
				vector<string> vals = split(line,',');
				for(int i=0;i<vals.size();i++)
				{
					if(isFloat(vals[i]))
					{
						salinity.push_back(stof(vals[i]));
					}
					else
					{
						salinity.push_back(0.0);
					}
				}
				getline (in,line);
				stringClean(line);
			  }
		  }
		}
		XAxis = xvalues.size();
		YAxis = yvalues.size();
		ZAxis = zvalues.size();
		in.close();
	}

	else 
		cout << "Unable to open file"; 
}

vector<string> DataHandler::split(const string &s, char delim)
{
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

vector<string>& DataHandler::split(const string &s, char delim, vector<string> &elems) 
{
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

bool DataHandler::isFloat( string myString ) 
{
    istringstream iss(myString);
    float f;
    iss >> noskipws >> f;
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail(); 
}

void DataHandler::stringClean(string& s)
{
	s.erase(remove(s.begin(), s.end(), ' '), s.end());
	s.erase(remove(s.begin(), s.end(), '\t'), s.end());
	replace( s.begin(), s.end(), '=', ',');
	replace( s.begin(), s.end(), ';', ',');
}
