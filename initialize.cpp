//CONTAGION - CSCE2110.201 - Group 10
#include "initialize.h"

void initialize::readConfig(string configName){
    ifstream configFile; //open

    configFile.open(configName);		                												//open config
    while(!configFile.is_open()){			                											//check if open
        cout << configName << " is an invalid file name. Please enter a new name: " << endl;			//prompt user for new name and retry opening
        cin >> configName;
        configFile.open(configName);
    }

	//reads in all info and store in private variables
    configFile.ignore(20, ':');
    configFile >> popFilename;
    configFile.ignore(20, ':');
    configFile >> regionFilename;
    configFile.ignore(20, ':');
    configFile >> infArea;
    configFile.ignore(20, ':');
    configFile >> infPeriod;
    configFile.ignore(20, ':');
    configFile >> contactRate;
    configFile.ignore(20, ':');
    configFile >> numVaccines;
	
    configFile.close(); //close
}


void initialize::readPop(){
    ifstream popFile;	//open
	
	popFile.open(popFilename);		  //check if opened properly              										
	if(!popFile.is_open()){				                					
        cout << "Could not find population file " << popFilename << ". Exiting program..."<< endl;
        exit (EXIT_FAILURE);																		
    }

	int areaID;
	int areaPop;
	int temp;

	while(!popFile.eof()){		//reading data
		popFile >> areaID;
		popFile.ignore(1, ':');
		popFile >> areaPop;
		if(temp != areaID){	//duplicate issue at end of read, idk why but this fixes it
			area.push_back(new regionData(areaID, areaPop, 0));	//adds new area with population to vector
		}
		temp = areaID;
    }
	
    popFile.close(); //close
}

void initialize::readRegion(){
    ifstream regionFile;//open
	
	regionFile.open(regionFilename);	//check if file opened properly	                											
	if(!regionFile.is_open()){				                					
        cout << "Could not find region file " << regionFilename << ". Exiting program..."<< endl;
        exit (EXIT_FAILURE);																		
    }
	
	char data;
	int areaCount;
	
	regionFile.ignore(100, '\n');	//ignore header row
	while(!regionFile.eof()){
		for(int i = 0; i < area.size(); i++){
			areaCount = 1;
			regionFile.get(data);	//ignores header column
			while(data != '\n'){
				regionFile.get(data);
				if(data == '1'){//adds area to adjacency vector for current area if csv data is 1
					area[i]->addAdjacent(areaCount / 2);	//error with doubling data due to .get twice? dividing by two fixes it, but might rework later 
				}
				areaCount = areaCount + 1;
			}
		}
	}

	regionFile.close();//close
}

void initialize::printInitial(){
	cout << "Regional Population" << endl;
	for(int i = 0; i < area.size(); i++){	//prints regional population
		area[i]->createAgents();
		cout << area[i]->getID() << ". " << area[i]->countAgents() << endl;
	}
	
	cout << endl << "Adjacency List" << endl;
	for(int i = 0; i < area.size(); i++){	//prints adjacency list
		cout << area[i]->getID() << ": ";
		area[i]->printAdjacent();
	}
	cout << endl;
}

int initialize::totalCount(char type){//counts the number of a specified type in all areas of the region
	int total = 0;
	for(int i = 0; i < area.size(); i++){
		total = total + area[i]->countType(type);
	}
	return total;
}

int initialize::getVaccines()
{
	return numVaccines;
}

void initialize::simulate(int simNum){
	
	int infTotal = 0;	//new infected count for analysis
	int currInfected = 1;	//in total region
	int infected;	//in each area
	int day = 0; //day counter
	
	//create new analysis object
	distAnalysis.push_back(new analysis());
	
	//initialized first infected in specified area
	area[infArea - 1]->changeState(1,'S', 'I');
	
	//TEST VACCINATION VALUES--COMMENT OUT 
	/*area[0]->changeState(2938, 'S', 'V');
	area[2]->changeState(2938, 'S', 'V');
	area[3]->changeState(2938, 'S', 'V');
	area[4]->changeState(2000, 'S', 'V');
	area[1]->changeState(2938, 'S', 'V');
	area[5]->changeState(2937, 'S', 'V');
	area[6]->changeState(2937, 'S', 'V');
	area[7]->changeState(2937, 'S', 'V');
	area[8]->changeState(2937, 'S', 'V');*/
	
	while(currInfected != 0){//end simulation when infectious agents = 0
		currInfected = totalCount('I');	//update infectious agents
		cout << "DAY " << day << endl;		//output day
		
		//get data for analysis - last day, peak infected/day
		distAnalysis[simNum]->setEndDay(day);
		if(currInfected > distAnalysis[simNum]->getPeakInf()){
			distAnalysis[simNum]->setPeakInf(currInfected);
			distAnalysis[simNum]->setPeakDay(day);
		}

		for(int i = 0; i < area.size(); i++){	//update every area
			infected = 0;
			
			//get data for analysis - total infected
			infTotal = area[i]->countNewInf() + infTotal;
			distAnalysis[simNum]->setTotalInf(infTotal);
			
			area[i]->updateTime();//updates time infected
			
			//output APSIRV for day
			cout << area[i]->getID() << ". POP: " << setw(7) << left << area[i]->countAgents();
			cout << " S: " << setw(7) << left << area[i]->countType('S') << " I: " << setw(7) << left << area[i]->countType('I');
			cout << " R: " << setw(7) << left << area[i]->countType('R') << " V: " << setw(7) << left << area[i]->countType('V');
			cout << endl;
			
			//update areas for next day
			//infect area
			infected = area[i]->countType('I');
			if(area[i]->countType('I') > 0){
				for(int j = 0; j < infected; j++){
					area[i]->changeState(contactRate,'S','I');
				}
			}
			
			//infect adjacent
			if(infected > (area[i]->getPop() / 2)){
				for(int k = 0; k < area[i]->getAdjacentSize(); k++){
					if(area[area[i]->getAdjacents(k)]->countType('I') == 0){
						area[area[i]->getAdjacents(k)]->changeState(1, 'S', 'I');
					}
				}
			}
			
			//recover
			area[i]->changeState(area[i]->countRecoveries(infPeriod), 'I', 'R');	//changes state to recovered for number of infected at infPeriod
		}
		
		day = day + 1;//next day
		cout << endl;
	}
	
	//reset simulation to initial values for next run
	for(int i = 0; i < area.size(); i++){
		area[i]->resetAgents();
	}
}

void initialize::closenessDistribution(){
	/*vector<float> closenessRank;
	vector<int> areaRank;
	vector<int> temp;
	
	float preAvg = 0;
	float closeness = 0;
	float size = area.size();
	int num = 1;
	int num2 = 0;
	int k = 0;
	for(int i = 0; i < area.size(); i++){
		closeness = 0;
		preAvg = 0;
		num = 1;
		num2 = 0;
		k = 0;
		for(int j = 0; j < area.size(); j++){
			num2 = i;
			while(area[num2]->getAdjacents(k) != j){
				num = num + 1;
				k++;
				if(k == area[num2]->getAdjacentSize()){
					num2 = num2 + 1;
				}
			}
		}
		temp.push_back(num);
		for(int j = 0; j < temp.size(); j++){
			preAvg = preAvg + temp[j];
		}
		temp.clear();
		closeness = preAvg / size;
		closenessRank.push_back(closeness);
		areaRank.push_back(i);
	}
	for(int i = 0; i < closenessRank.size(); i++){	//DEBUG OUTPUT
		cout << setprecision(5) << closenessRank[i];
	}cout << endl;
	for(int i = 0; i < areaRank.size(); i++){
		cout << areaRank[i] + 1;
	}*/
}

int initialize::highestDeg()
{
	int maxDegPop;
	int maxID;

	for(int i = 0; i < area.size(); i++)
	{
		if (area[i]->getAdjacentSize() > area[i+1]->getAdjacentSize())
		{
			maxID = area[i]->getID();
			//maxDegPop = area[i];
			break;
		}
		else if (area[i]->getAdjacentSize() == area[i+1]->getAdjacentSize())
		{
			if	(area[i]->getID() < area[i+1]->getID())
			{
				maxID = area[i]->getID();
				//maxDegPop = area[i];
				break;
			}
			else
			{
				//maxDegPop = area[i+1]->getPop();
				maxID = area[i+1]->getID();
				//maxDegPop = area[i+1];
				break;
			}
		}
		else
		{
			//maxDegPop = area[i+1]->getPop();
			maxID = area[i+1]->getID();
			//maxDegPop = area[i+1];
			break;
		}

	}

		for (auto it = area.begin(); it != area.end();)
		{
        if (*it == area[maxID]) 
		{
            it = area.erase(it);
        } 
		else 
		{
            ++it;
        }
	}
	return maxID;
}

void initialize::randomDistribution(){
	int toVacc = numVaccines;
	int areaNum = 0;
	while(toVacc > 0){
		area[areaNum]->changeState(toVacc,'S','V');
		toVacc = toVacc - area[areaNum]->countType('V');
		areaNum = areaNum + 1;
	}
}

void initialize::degreeDistribution(int vax)
{	
	/*int numVax = vax;

	int maxDegPop;
	int maxID;
	int maxSize;

	for(int i = 0; i < area.size(); i++)
	{
		//cout << "Area " << i+1 << ": " << area[i]->getAdjacentSize() << endl; //prints out area number and adjacent size
		cout << highestDeg() << endl;
	}

	cout << highestDeg() << endl;


	cout << "--------------------------------------------" << endl;
	cout << "Area 1: " << area[0]->getAdjacentSize() << endl;
	cout << "Area 2: " << area[1]->getAdjacentSize() << endl;
	cout << "Area 3: " << area[2]->getAdjacentSize() << endl;
	cout << "Area 4: " << area[3]->getAdjacentSize() << endl;
	cout << "Area 5: " << area[4]->getAdjacentSize() << endl;
	cout << "Area 6: " << area[5]->getAdjacentSize() << endl;
	cout << "Area 7: " << area[6]->getAdjacentSize() << endl;
	cout << "Area 8: " << area[7]->getAdjacentSize() << endl;
	cout << "Area 9: " << area[8]->getAdjacentSize() << endl;

	//while (numVax > 0)
	//{
		//highestDeg();
	
		
    //}

//		numVax -= maxDegPop;

		//cout << "Area " << maxID << ": " << maxDegPop;
		//cout << numVax << endl;*/
	vector<int> degreeRank;
	vector<int> areaRank;
	for(int i = 0; i < area.size(); i++){
		degreeRank.push_back(area[i]->getAdjacentSize());
		areaRank.push_back(i);
	}
	for(int i = 0; i < degreeRank.size(); i++){
		for(int j = 0; j < degreeRank.size(); j++){
			if(degreeRank[j] < degreeRank[i]){
				swap(degreeRank[j], degreeRank[i]);
				swap(areaRank[j], areaRank[i]);
			}else if(degreeRank[j] == degreeRank[i] && areaRank[j] > areaRank[i]){
				swap(degreeRank[j], degreeRank[i]);
				swap(areaRank[j], areaRank[i]);
			}
		}
	}
	/*for(int i = 0; i < degreeRank.size(); i++){	//DEBUG OUTPUT
		cout << degreeRank[i];
	}cout << endl;
	for(int i = 0; i < areaRank.size(); i++){
		cout << areaRank[i] + 1;
	}*/
	int toVacc = numVaccines;
	int areaNum = 0;
	while(toVacc > 0){
		area[areaRank[areaNum]]->changeState(toVacc,'S','V');
		toVacc = toVacc - area[areaRank[areaNum]]->countType('V');
		areaNum = areaNum + 1;
	}
}

void initialize::equalDistribution(int vax)
{
	/*cout << vax << endl;
	int numVax = vax;

	while(numVax > 0)
	{
		for(int i = 0; i < area.size(); i++)
		{
			int maxVax = area[i]->getPop();
			
			if (maxVax != area[i]->getVax())
			{
				area[i]->giveVax();
				numVax = numVax - 1;
			}
		}
	}

	for(int i = 0; i < area.size(); i++)
	{	
		area[i]->changeState(area[i]->getVax(), 'S', 'V');		//change state of agents

		cout << area[i]->getVax() << endl;						//test to see values
	}*/
	int countVacc = 0;
	int toVacc = numVaccines;
	while(toVacc > 0){
		for(int j = 0; j < area.size(); j++){
			if(area[j]->getPop() != area[j]->countType('V') && toVacc > 0){
			area[j]->changeState(1, 'S', 'V');
			countVacc = countVacc + 1;
			toVacc = toVacc - 1;
			}
		}
	}
}

void initialize::analyze(){
	//cout << "Using the closeness centrality distribution method, the peak number of infected was " << distAnalysis[0]->getPeakInf() << " on day " << distAnalysis[0]->getPeakDay() << ". The outbreak ended on day "<< distAnalysis[0]->getEndDay() << " and the total number of infected infividuals was " << distAnalysis[0]->getTotalInf() << "." << endl;
	cout << "Using the degree centrality distribution method, the peak number of infected was " << distAnalysis[1-1]->getPeakInf() << " on day " << distAnalysis[1-1]->getPeakDay() << ". The outbreak ended on day "<< distAnalysis[1-1]->getEndDay() << " and the total number of infected infividuals was " << distAnalysis[1-1]->getTotalInf() << "." << endl;
	cout << "Using the random distribution method, the peak number of infected was " << distAnalysis[2-1]->getPeakInf() << " on day " << distAnalysis[2-1]->getPeakDay() << ". The outbreak ended on day "<< distAnalysis[2-1]->getEndDay() << " and the total number of infected infividuals was " << distAnalysis[2-1]->getTotalInf() << "." << endl;
	cout << "Using the equal distribution method, the peak number of infected was " << distAnalysis[3-1]->getPeakInf() << " on day " << distAnalysis[3-1]->getPeakDay() << ". The outbreak ended on day "<< distAnalysis[3-1]->getEndDay() << " and the total number of infected infividuals was " << distAnalysis[3-1]->getTotalInf() << "." << endl;
}


























