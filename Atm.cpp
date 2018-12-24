/*
 * Atm.cpp
 *
 *  Created on: Dec 17, 2018
 *      Author: os
 */

#include "Atm.h"
#include "Account.h"

const string cmdO = "O";
const string cmdL = "L";
const string cmdD = "D";
const string cmdW = "W";
const string cmdB = "B";
const string cmdQ = "Q";
const string cmdT = "T";

using namespace std;

extern map<string, Account> accountsMap;
extern bool comissionDone;

Account banksAccount("Bank", "0000", 0);

Atm::Atm() :
		serialNumber(-1), inputFile("") {
}
// TODO Auto-generated constructor stub

Atm::Atm(int serialNum, std::string fileName) :
		serialNumber(serialNum), inputFile(fileName) {
}


void* printAnimation(void*) {
	//while (comissionDone == false) {
		// Go to sleep in every action
		usleep(print_sleep_time);
		printf("\033[2J");
		printf("\033[1;1H");

		cout << "Current Bank Status" << endl;
		for (map<string, Account>::iterator it = accountsMap.begin();
				it != accountsMap.end(); ++it) {
			cout << "Account " << it->second.getID() << ": Balance - "
					<< it->second.getBalance() << " $ , Account Password - "
					<< it->second.getPass() << endl;
		}
		cout << "The Bank has " << banksAccount.getBalance() << " $" << endl;
		// readers ADT exit
	//}
	return NULL;
}

void* takeComission(void*)
{
	ofstream logWriter;
	logWriter.open("log.txt", ifstream::app);
	int percent = 0;
	//while (comissionDone == false) {
		sleep(3);
		/*if (comissionDone == true) {
			break;
		}*/
		// rand the commission percent 2 -4 %
		srand(time(NULL));
		percent = (rand() % 3) + 2;
		for (map<string, Account>::iterator it = accountsMap.begin();
				it != accountsMap.end(); ++it) {
			int balance = it->second.getBalance();
			int fee =
					(int)(round(((double)percent / 100) * (double) balance));
			int newBalance = balance - fee;
			it->second.setBalance(newBalance);
			string accId = it->second.getID();
			int bankNewBalance = banksAccount.getBalance();
			bankNewBalance += fee;
			banksAccount.setBalance(bankNewBalance);
			logWriter << "Bank: " << "commissions of " << percent
					<< " % were charged, the bank gained " << fee
					<< " $ from account " << accId << endl;
		}
	//}
	logWriter.close();
	return NULL;
}


void* atmExeCommandsFunc(void* cur_atm) {

	Atm* atm = static_cast<Atm*>(cur_atm);

	/************** Get Input File ****************/
	string fileName = atm->getInputName();
	ifstream inputReader;
	inputReader.open(fileName.c_str());

	ofstream logWriter;
	logWriter.open("log.txt", ifstream::app);

	string line;
	char *saveptr;
	const char *delimiters = " \t\n";
	unsigned int sleepTime = atm_sleep_time;
	//command line
	char *cmd = nullptr;
	string args[5];
	int cmdLength = 0;
	//account num and password as strings
	string accountNum("");
	string password("");

	/************** Read Line ****************/
	while (getline(inputReader, line)) {
		//delay between two commands
		usleep(sleepTime);
		char *saveptr = NULL;
		cmdLength = line.length();
		cmd = new char[1 + cmdLength];
		cmd[cmdLength] = '\0';
		strcpy(cmd, line.c_str());
		args[0] = string(strtok_r(cmd, delimiters, &saveptr));
		// Get each argument, the first is the command sign
		// The second is account number and the third is password
		for (unsigned int i = 1; i < 5; i++) {
			try {
			args[i] = string(strtok_r(NULL, delimiters, &saveptr));
			} catch (...) {
				break;
			}
		}
		//strtok_r output to strings
		accountNum = args[1];
		password = args[2];

		/************** Perform Commands ****************/
		/************** cmdO - Open an account ****************/
		if (args[0] == cmdO) {
			// open account
			// if account already exists in map
			if (accountsMap.count(accountNum) > 0) {
				//log lock
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account with the same id exists"
						<< endl;
				sleep(1);
				delete[] cmd;
				continue;
			}
			string initialBalance = args[3];
			int balance = stoi(args[3]);
			if (balance < 0) {
				cerr << "Request to open account with negative balance" << endl;
				sleep(1);
				delete[] cmd;
				continue;
			}
			Account newAccount = Account(accountNum, password, balance);
			accountsMap.insert(pair<string, Account>(accountNum, newAccount));
			// log lock
			logWriter << atm->getSerial() << ": New account id is "
					<< accountNum << " with password " << password
					<< " and initial balance " << initialBalance << endl;
			sleep(1);
		}
		/************** cmdL - account VIP ****************/
		else if (args[0] == cmdL) {

			// if account already exists in map
			if (accountsMap.count(accountNum) == 0) {

				//log lock
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - password for account id "
						<< accountNum << " is incorrect" << endl;
				sleep(1);
				delete[] cmd;
				continue;
			}
			Account accountEnter = accountsMap.at(accountNum);

			//check correct password
			if (password != accountEnter.getPass()) {
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - password for account id "
						<< accountNum << " is incorrect" << endl;
				sleep(1);
				delete[] cmd;
				continue;
			}
			accountEnter.setVIP();
			sleep(1);
			delete[] cmd;
			continue;
		}
		/************** cmdD - Deposit ****************/
		else if (args[0] == cmdD) {
			if (accountsMap.find(accountNum) == accountsMap.end()) {
				//account doesn't exist
				sleep(1);
				// log mutex
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNum << " does not exist" << endl;
				delete[] cmd;
				continue;
			}
			Account& curAcc = accountsMap.at(accountNum);
			//check correct password
			if (password != curAcc.getPass()) {
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - password for account id "
						<< accountNum << " is incorrect" << endl;
				sleep(1);
				delete[] cmd;
				continue;

			}
			int amount = stoi(args[3]);
			//negative amount
			if (amount < 0) {
				cerr << "Can't deposit negative amount" << endl;
				sleep(1);
				delete[] cmd;
				continue;
			}
			// add amount to account
			curAcc.setDeposit(amount);
			int newBalance = curAcc.getBalance();
			logWriter << atm->getSerial() << ": Account " << accountNum
					<< " new balance is " << newBalance << " after amount "
					<< amount << " was withdrew" << endl;
			delete[] cmd;
			sleep(1);
		}
		/************** cmdW - Withdraw ****************/
		else if (args[0] == cmdW) {
			if (accountsMap.find(accountNum) == accountsMap.end()) {
				//account doesn't exist
				sleep(1);
				// log mutex
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNum << " does not exist" << endl;
				delete[] cmd;
				continue;
			}
			Account& curAcc = accountsMap.at(accountNum);

			//check correct password
			if (password != curAcc.getPass()) {
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - password for account id "
						<< accountNum << " is incorrect" << endl;
				sleep(1);
				delete[] cmd;
				continue;
			}
			int amount = stoi(args[3]);
			//negative amount
			if (amount < 0) {
				cerr << "Can't withdraw negative amount" << endl;
				sleep(1);
				delete[] cmd;
				continue;
			}
			// withdraw amount from account
			if (curAcc.setWithdrawal(amount) == true) {
				int newBalance = curAcc.getBalance();
				logWriter << atm->getSerial() << ": Account " << accountNum
						<< " new balance is " << newBalance << " after amount "
						<< amount << " was withdrew" << endl;
				sleep(1);
				delete[] cmd;
				continue;
			} else {
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNum << "balance is lower than " << amount
						<< endl;
				sleep(1);
				delete[] cmd;
				continue;
			}
			/************** cmdB - Get Balance ****************/
		} else if (args[0] == cmdB) {
			if (accountsMap.find(accountNum) == accountsMap.end()) {
				//account doesn't exist
				sleep(1);
				// log mutex
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNum << " does not exist" << endl;
				delete[] cmd;
				continue;
			}
			//account exists, checking password
			Account curAcc = (accountsMap.find(accountNum))->second;
			if(curAcc.getPass() != password) {
				//wrong password
				sleep(1);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - password for account id "
						<< accountNum << " is incorrect" << endl;
				delete[] cmd;
				continue;
			}
			int curBalance = curAcc.getBalance();
			sleep(1);
			logWriter << atm->getSerial() << ": Account " << accountNum
					<< " balance is " << curBalance << endl;
			delete[] cmd;

			/************** cmdT - Make transaction ****************/
		} else if(args[0] == cmdT) {
			string accountNumDest = args[3];
			string transAmountStr = args[4];
			int transAmount = stoi(transAmountStr);
			if(accountNumDest == accountNum) {
				//acc src and dest are the same
				delete[] cmd;
				continue;
			}
			if (accountsMap.find(accountNum) == accountsMap.end()) {
				//account doesn't exist
				sleep(1);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNum << " does not exist" << endl;
				delete[] cmd;
				continue;
			}
			//account exists, checking password
			Account& srcAcc = (accountsMap.find(accountNum))->second;
			if (srcAcc.getPass() != password) {
				//wrong password
				sleep(1);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - password for account id "
						<< accountNum << " is incorrect" << endl;
				delete[] cmd;
				continue;
			}
			//src password valid, check dst account exists
			if (accountsMap.find(accountNumDest) == accountsMap.end()) {
				//account doesn't exist
				sleep(1);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNumDest << " does not exist" << endl;
				delete[] cmd;
				continue;
			}
			int balance = srcAcc.getBalance();
			int newBalance = balance - transAmount;
			if (newBalance < 0) {
				sleep(1);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNum << " balance is lower than "
						<< transAmount << endl;
				delete[] cmd;
				continue;
			}
			accountsMap.find(accountNum)->second.setBalance(newBalance);
			Account& dstAccount = (accountsMap.find(accountNumDest))->second;
			int dstNewBalance = dstAccount.getBalance();
			dstNewBalance += transAmount;
			(accountsMap.find(accountNumDest))->second.setBalance(
					dstNewBalance);
			sleep(1);
			logWriter << atm->getSerial() << ": Transfer " << transAmount
					<< " from account " << accountNum << " to account "
					<< accountNumDest << " new account balance is "
					<< newBalance << " new target account balance is "
					<< dstNewBalance << endl;
			delete[] cmd;
		}
	}
	return nullptr;
}
