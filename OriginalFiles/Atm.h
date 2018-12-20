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
	Atm(int serialNum, std::string fileName): serialNumber(serialNum), inputFile(fileName) {

	}
	virtual ~Atm();

	void* atmExeCommandsFunc(void*);

};

#endif /* ATM_H_ */
