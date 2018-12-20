/*
 * Atm.cpp
 *
 *  Created on: Dec 17, 2018
 *      Author: os
 */

#include "Atm.h"
#include "Account.h"

using namespace std;

Atm::Atm() {
	// TODO Auto-generated constructor stub

}

Atm::~Atm() {
	// TODO Auto-generated destructor stub
}

void* atmExeCommandsFunc(void* _atm) {

	Atm* atm = static_cast<Atm*>(atm);
	ifstream fileReader(atm->inputFile);
	ofstream fileWriter(atm->inputFile);


}
