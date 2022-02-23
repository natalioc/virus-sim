//CONTAGION - CSCE2110.201 - Group 10
#include "initialize.h"
#include "regiondata.h"
#include "agent.h"
#include <iostream>
#include <string>

using namespace std;

int main() {

    string fileName;


    initialize initial;

	//call functions to read in all files
    cout << "Please enter the name of the configuration file:";
    cin >> fileName;
    initial.readConfig(fileName);
	initial.readPop();
	initial.readRegion();

	//print initial region population and adjacency lists
	initial.printInitial();

	int numVax = initial.getVaccines();

	//begin simulation
	int simulationNumber = 0;
	cout << "CLOSENESS DISTRIBUTION" << endl;
	//initial.closenessDistribution(); //distribute vaccines to population
	//initial.simulate(simulationNumber);	//runs simulation; simulation number used for analysis
	
	//simulationNumber = simulationNumber + 1;
	cout << "DEGREE DISTRIBUTION" << endl;
	initial.degreeDistribution(numVax);
	initial.simulate(simulationNumber);
	
	simulationNumber = simulationNumber + 1;
	cout << "RANDOM DISTRIBUTION" << endl;
	initial.randomDistribution();
	initial.simulate(simulationNumber);
	
	simulationNumber = simulationNumber + 1;
	cout << "EQUAL DISTRIBUTION" << endl;
	initial.equalDistribution(numVax);
	initial.simulate(simulationNumber);
	
	//output analysis of all distributions
	initial.analyze();
	
    return 0;
}
