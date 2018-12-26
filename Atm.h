/*
 * Atm.h
 *
 *  Created on: Dec 17, 2018
 *      Author: os
 */

#ifndef ATM_H_
#define ATM_H_

#include "Account.h"

class Atm {

private:

	int serialNumber;
	std::string inputFile;

public:
	Atm();
	Atm(int serialNum, string fileName);

	~Atm() = default;


	std::string getInputName() {
		return this->inputFile;
	}

	int getSerial() {
		return this->serialNumber;
	}

};

void* printAnimation(void*);

void* takeComission(void*);

void* atmExeCommandsFunc(void* cur_atm);


#endif /* ATM_H_ */
