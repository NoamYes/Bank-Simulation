

#include "Account.h"
#include "Atm.h"
#include <map>
#include <vector>

using namespace std;


map<string, Account> accountsMap;
bool comissionDone;


int main(int argc, char *argv[]) {

	vector<Atm*> atmsVector;
	// if there are no arguments at all
	if (argc == 0 || argc == 1) {
		cerr << "illegal arguments" << endl;
		return -1;
	}

	int atmNum = atoi(argv[1]);
	if (argc - 2 != atmNum) {
		cerr << "illegal arguments" << endl;
		return -1;
	}
	comissionDone = false;
	Atm* atm = new Atm(1, "short_atm.txt");
	ofstream{"log.txt"};
	// Insert each ATM's input file

	/*for (int i = 0; i < atmNum; i++) {
		string fileName = string(argv[2 + i]);
		// construct ATM with serial number and input file
		Atms[i] = Atm(i, fileName);
	}*/
	//pthread_t *atmThread = new pthread_t;

	//pthread_create(atmThread, NULL, atmExeCommandsFunc, static_cast<void*>(atm));
	//pthread_join(*atmThread, NULL);
	atmExeCommandsFunc(atm);
	printAnimation(NULL);
	takeComission(NULL);
	printAnimation(NULL);
	return 0;

}
