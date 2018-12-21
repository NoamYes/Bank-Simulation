

#include "Account.h"

using namespace std;


map<int, Account> accountsMap;







int main(int argc, char *argv[]) {

	Atm* Atms;


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

	Atms = new Atm[atmNum];

	// Insert each ATM's input file

	for(int i = 0; i < atmNum; i++) {

		string fileName = string(argv[2+i]);
		// construct ATM with serial number and input file
		Atms[i] = Atm(i, fileName);

	}

return 0;

}


