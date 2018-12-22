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
Atm::Atm(int serialNum, std::string fileName): serialNumber(serialNum), inputFile(fileName) {}

Atm::~Atm() {
	// TODO Auto-generated destructor stub
}

void* atmExeCommandsFunc(void* t_atm) {

	Atm* atm = static_cast<Atm*>(t_atm);

	    string fileName = atm->getInputName();
	    ifstream inputReader;
	    inputReader.open(fileName.c_str());

	    ofstream logWriter;
	    logWriter.open("log.txt", ifstream::app);

	    // Line of input file
	    string line;
	    char *saveptr;
	    const char *delimiters = " \t\n";
	    unsigned int sleepTime = ATMSLEEP;

	    // iterate over lines in input file
		while (getline(inputReader, line))
		{
			// delay between two commands
			usleep(sleepTime);

			const char *args[5];	// args[0] is the command name
			int argc = 0;
			const char cmdO[] = "O";
			const char cmdL[] = "L";
			const char cmdD[] = "D";
			const char cmdW[] = "W";
			const char cmdB[] = "B";
			const char cmdQ[] = "Q";
			const char cmdT[] = "T";

			// Handle const char* problem that strtok_r had
			int cmdLength = (int)strlen(line.c_str());
			char *cmd = new char[1 + cmdLength];
			cmd[cmdLength] = '\0';
			strcpy(cmd, line.c_str());
			args[0] = strtok_r(cmd, delimiters, &saveptr);

			// Get each argument, the first is the command sign
			// The second is account number and the third is password
			for(unsigned int i = 1; i < 5; i++)
			{
				args[i] = strtok_r(NULL, delimiters, &saveptr);
				argc++;
			}

			// strtok_r outpot and atoi input is const char*
			const char* accountNumStr = args[1];
			const char* passwordStr = args[2];

			// Convert arguments 1 and 2 into cpp string
			string accountNumString(accountNumStr);
			string passwordNumString(passwordStr);

			int accountNum = atoi(accountNumStr);
			int password = atoi(passwordStr);

			// Illegal argument
			if(accountNum < 0)
			{
				sleep(1);

				// handle error with account - transaction failed
				// log mutex
//				pthread_mutex_lock(&logMutex);
//				fHandleWrite << "Error " << atm->getID()
//				<< ": Your transaction failed - account id " << accountNumString << " does not exist" << endl;
//				pthread_mutex_unlock(&logMutex);
				// log mutex
				delete[] cmd;
				continue;
			}

			// from here COMMANDS executor

			if(*args[0] == *cmdO) {
				// open account

				// if account already exists in map
				if (accountsMap.count(accountNum) > 0) {

					//log lock
					logWriter << "Error " << atm->getSerial() <<
							": Your transaction failed - account with the same id exists" << endl;
					sleep(1);
					continue;
				}

				const char* initialBalance = args[3];
				string initialBalanceStr(initialBalance);
				int Balance = atoi(args[3]);
				if (Balance < 0) {
					cerr << "Request to open account with negative balance" << endl;
					sleep(1);
					continue;
				}
				Account newAccount = Account(accountNum, password, initialBalance);
				accountsMap.insert(pair<int,Account>(accountNum,newAccount));
				// log lock
				logWriter << atm->getSerial() <<
						": New account id is " << accountNumStr << " with password "
						<< passwordNumString << " and initial balance "
						<< initialBalanceStr << endl;

				sleep(1);


			}

			// make account a vip account
			else if (*args[0] == *cmdL) {

				// if account already exists in map
				if (accountsMap.count(accountNum) == 0) {

					//log lock
					logWriter << "Error " << atm->getSerial() <<
							": Your transaction failed - password for account id " << accountNumStr
							<< " is incorrect" << endl;
					sleep(1);
					continue;
				}

				Account accountEnter = accountsMap.at(accountNum);

				//check correct password
				if (password != accountEnter._password ) {
					logWriter << "Error " << atm->getSerial() <<
							": Your transaction failed - password for account id " << accountNumStr
							<< " is incorrect" << endl;
					sleep(1);
					continue;

				}

				accountEnter.setVIP();
				sleep(1);
				continue;

			}

			// Deposit command
			else if (*args[0] == *cmdD) {

				Account accountEnter = accountsMap.at(accountNum);

				//check correct password
				if (password != accountEnter._password ) {
					logWriter << "Error " << atm->getSerial() <<
							": Your transaction failed - password for account id " << accountNumStr
							<< " is incorrect" << endl;
					sleep(1);
					continue;

				}

				int amount = atoi(args[3]);
				//negative amount
				if (amount < 0) {
					cerr << "Can't deposit negative amount" << endl;
					sleep(1);
					continue;
				}

				// add amount to account
				accountEnter.setDeposit(amount);
				int newBalance = accountEnter.getBalance();
				logWriter << atm->getSerial() << ": Account " << accountNumStr
						<< " new balance is " << newBalance << " after amount "
						<< amount << " was withdrew" << endl;

				sleep(1);
			}

			else if (*args[0] == *cmdW) {

				Account accountEnter = accountsMap.at(accountNum);

				//check correct password
				if (password != accountEnter._password ) {
					logWriter << "Error " << atm->getSerial() <<
							": Your transaction failed - password for account id " << accountNumStr
							<< " is incorrect" << endl;
					sleep(1);
					continue;

				}

				int amount = atoi(args[3]);
				//negative amount
				if (amount < 0) {
					cerr << "Can't withdraw negative amount" << endl;
					sleep(1);
					continue;
				}

				// withdraw amount from account
				if (accountEnter.setWithdrawal(amount) == true) {
					int newBalance = accountEnter.getBalance();
					logWriter << atm->getSerial() << ": Account " << accountNumStr
							<< " new balance is " << newBalance << " after amount "
							<< amount << " was withdrew" << endl;
					sleep(1);
					continue;
				}
				else {
					logWriter << "Error " << atm->getSerial() <<
							": Your transaction failed - account id " << accountNumStr
							<< "balance is lower than " << amount << endl;
					sleep(1);
					continue;
				}





			}



		}

































	}




}
