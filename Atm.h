/*
 * Atm.h
 *
 *  Created on: Dec 17, 2018
 *      Author: os
 */

#ifndef ATM_H_
#define ATM_H_

#include "Account.h"

using namespace std;

class Atm {

private:

	int serialNumber;
	string inputFile;

public:
	Atm();
	Atm(int serialNum, std::string fileName);

	virtual ~Atm();

	void* atmExeCommandsFunc(void*);

	string getInputName() {
		return this->inputFile;
	}

	int getSerial() {
		return this->serialNumber;
	}

};

#endif /* ATM_H_ */
