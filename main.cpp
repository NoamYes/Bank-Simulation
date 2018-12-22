
#include "Atm.h"
#include "Account.h"
#include <vector>

using namespace std;


map<int, Account> accountsMap;


/*************************** GLOBALS ************************/
//TODO initialize global threads mutexes  and variables




int main(int argc, char *argv[]) {

	vector<Atm*> atmsVector;

	// init threads mutexes
	// if there are no arguments at all
	if(argc == 0 || argc == 1) {
		cerr << "illegal arguments" << endl;
		return -1;
	}

	int atmNum = atoi(argv[1]);
	if(argc-2 != atmNum) {
		cerr << "illegal arguments" << endl;
		return -1;
	}


	// TODO init print and comission threads
	// TODO init log file

	// Insert each ATM's input file
	// TODO create threads array to each ATM and init them in this for
	pthread_t* atmThreads = new pthread_t[atmNum];

	for(int i = 0; i < atmNum; i++) {

		string fileName = string(argv[2+i]);
		// construct ATM with serial number and input file
		Atm* newAtm = Atm(i, fileName);
		atmsVector.push_back(newAtm);
		int threadCreateErr = pthread_create(&atmThreads[i], NULL, atmExeCommandsFunc, (void*)&atmsVector.end());

	}


	// TODO join atm threads

	// TODO join print and comission threads



	//destroy all

return 0;

}


