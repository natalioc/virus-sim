//CONTAGION - CSCE2110.201 - Group 10
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include "regiondata.h"
#include "agent.h"
#include "analysis.h"

#ifndef INITIALIZATION_H
#define INITIALIZATION_H

using namespace std;

class initialize {
public:
    void readConfig(string configName);	//functions for reading files
    void readRegion();
    void readPop();
	
	void printInitial();	//function to print initial region population and adjacency list
	
	int totalCount(char type); //counts the number of a specified type in all areas of the region
    int getVaccines();         //returns the total number of vaccines

	void closenessDistribution();
    void degreeDistribution(int vax); //degree vaccine distribution
	bool checkArea(int areaNum);
    int highestDeg();                 //returns highest degree regiondata
	void randomDistribution();
    void equalDistribution(int vax);  //equal vaccine distribution
	
	void simulate(int simNum);	//runs SIRV simulation
	
	void analyze();		//prints analysis on each distribution
	
private:
    string regionFilename;	//filenames given in config
    string popFilename;
    int infArea;	//other data given in config.txt
    int infPeriod;
    int contactRate;
    int numVaccines;
	vector<regionData*> area; //stores region data in vector of regionData objects
	vector<analysis*> distAnalysis;


};

#endif
