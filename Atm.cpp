/*
 * Atm.cpp
 *
 *  Created on: Dec 17, 2018
 *      Author: os
 */

#include "Atm.h"
#include "Account.h"
#include <cassert>

const string cmdO = "O";
const string cmdL = "L";
const string cmdD = "D";
const string cmdW = "W";
const string cmdB = "B";
const string cmdT = "T";

using namespace std;

extern map<string, Account> accountsMap;
extern bool commissionDone;
extern int mapReadersNum;
extern pthread_mutex_t readersMutex;
extern pthread_mutex_t writersMutex;
extern pthread_mutex_t logMutex;
extern pthread_mutex_t bankMutex;

Account banksAccount("Bank", "0000", 0);

Atm::Atm() :
		serialNumber(-1), inputFile("") {
}

Atm::Atm(int serialNum, string fileName) :
		serialNumber(serialNum), inputFile(fileName) {
}

void readerEnter(pthread_mutex_t *curReadersMutex,
		pthread_mutex_t *curWritersMutex, int *readersCount) {
	//lock global readers to update count
	pthread_mutex_lock(curReadersMutex);
	(*readersCount)++;
	//first reader locks writers
	if (*readersCount == 1) {
		pthread_mutex_lock(curWritersMutex);
	}
	pthread_mutex_unlock(curReadersMutex);
}

void readerExit(pthread_mutex_t *curReadersMutex,
		pthread_mutex_t *curWritersMutex, int *readersCount) {
	//lock global readers to update count
	pthread_mutex_lock(curReadersMutex);
	(*readersCount)--;
	if (*readersCount == 0) {
		//last reader exits
		pthread_mutex_unlock(curWritersMutex);
	}
	pthread_mutex_unlock(curReadersMutex);
}

void writerEnter(pthread_mutex_t *currWritersMutex) {
	pthread_mutex_lock(currWritersMutex);
}

void writerExit(pthread_mutex_t *curWritersMutex) {
	pthread_mutex_unlock(curWritersMutex);
}

void* printAnimation(void*) {
	while (commissionDone == false) {
		//0.5 second sleep
		usleep(print_sleep_time);
		//enters global reader
		readerEnter(&readersMutex, &writersMutex, &mapReadersNum);
		printf("\033[2J");
		printf("\033[1;1H");
		cout << "Current Bank Status" << endl;
		for (map<string, Account>::iterator it = accountsMap.begin();
				it != accountsMap.end(); ++it) {
			//enters account
			readerEnter(&(it->second.accountReadersMutex),
					&(it->second.accountWritersMutex),
					&(it->second.numOfReaders));
			cout << "Account " << it->second.getID() << ": Balance - "
					<< it->second.getBalance() << " $ , Account Password - "
					<< it->second.getPass() << endl;
			//exits account
			readerExit(&(it->second.accountReadersMutex),
					&(it->second.accountWritersMutex),
					&(it->second.numOfReaders));
		}
		pthread_mutex_lock(&bankMutex);
		cout << "The Bank has " << banksAccount.getBalance() << " $" << endl;
		pthread_mutex_unlock(&bankMutex);
		//exits global reader
		readerExit(&readersMutex, &writersMutex, &mapReadersNum);
	}
	return NULL;
}

void* takeComission(void*) {
	ofstream logWriter;
	logWriter.open("log.txt", ifstream::app);
	int percent = 0;
	int bankNewBalance = 0;
	while (commissionDone == false) {
		sleep(3);
		//in case after the next 3 seconds we finished the program
		if (commissionDone == true) {
			break;
		} else {
			//rand commission from 2-4%
			srand(time(NULL));
			percent = (rand() % 3) + 2;
			//enter accounts map
			readerEnter(&readersMutex, &writersMutex, &mapReadersNum);
			for (map<string, Account>::iterator it = accountsMap.begin();
					it != accountsMap.end(); ++it) {
				if (it->second.isAccountVIP() == false) {
					//enter account write
					writerEnter(&(it->second.accountWritersMutex));
					int balance = it->second.getBalance();
					int fee = (int) (round(
							((double) percent / 100) * (double) balance));
					int newBalance = balance - fee;
					it->second.setBalance(newBalance);
					//exit account write
					writerExit(&(it->second.accountWritersMutex));
					//enter bank
					pthread_mutex_lock(&bankMutex);
					bankNewBalance = banksAccount.getBalance();
					//exit bank
					pthread_mutex_unlock(&bankMutex);
					bankNewBalance += fee;
					banksAccount.setBalance(bankNewBalance);
					//enter log mutex
					pthread_mutex_lock(&logMutex);
					logWriter << "Bank: " << "commissions of " << percent
							<< " % were charged, the bank gained " << fee
							<< " $ from account " << it->second.getID() << endl;
					//exit log mutex
					pthread_mutex_unlock(&logMutex);
				}
			}
			//exit accounts map
			readerExit(&readersMutex, &writersMutex, &mapReadersNum);
		}
	}
	logWriter.close();
	return NULL;
}

void* atmExeCommandsFunc(void* cur_atm) {

	Atm* atm = static_cast<Atm*>(cur_atm);
	//TODO delete after debug
	assert(atm);

	/************** Get Input File ****************/
	string fileName = atm->getInputName();
	ifstream inputReader;
	ofstream logWriter;
	//open read and write streams
	try {
		inputReader.open(fileName.c_str());
		logWriter.open("log.txt", ifstream::app);
	} catch (...) {
		cerr << "Error: Atm " << atm->getSerial() << " file open failed"
				<< endl;
	}
	string line("");
	const char *delimiters = " \t\n\r";
	unsigned int sleepTime = atm_sleep_time;
	//command line arguments
	char *cmd = NULL;
	string args[5];
	int cmdLen = 0;
	//account num and password variables
	string accountNum("");
	string password("");

	/************** Read Line ****************/
	while (getline(inputReader, line)) {
		//delay between two commands
		usleep(sleepTime);
		char *saveptr = NULL;
		cmdLen = line.length();
		cmd = new char[1 + cmdLen];
		cmd[cmdLen] = '\0';
		strcpy(cmd, line.c_str());
		args[0] = string(strtok_r(cmd, delimiters, &saveptr));
		//get command arguments
		for (int i = 1; i < 5; i++) {
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
			//lock map
			writerEnter(&writersMutex);
			// open account
			// if account already exists in map
			if (accountsMap.count(accountNum) > 0) {
				//enter log
				pthread_mutex_lock(&logMutex);
				sleep(1);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account with the same id exists"
						<< endl;
				//exit log
				pthread_mutex_unlock(&logMutex);
				//account doesn't exist
			} else {
				string initialBalance = args[3];
				int balance = stoi(args[3]);
				if (balance < 0) {
					//enter log
					pthread_mutex_lock(&logMutex);
					logWriter << "Error " << atm->getSerial()
							<< ": Request to open account with negative balance"
							<< endl;
					sleep(1);
					//exit log
					pthread_mutex_unlock(&logMutex);
				} else {
					//ballance is valid
					Account newAccount = Account(accountNum, password, balance);
					accountsMap.insert(
							pair<string, Account>(accountNum, newAccount));
					//enter log
					pthread_mutex_lock(&logMutex);
					sleep(1);
					logWriter << atm->getSerial() << ": New account id is "
							<< accountNum << " with password " << password
							<< " and initial balance " << initialBalance
							<< endl;
					//exit log
					pthread_mutex_unlock(&logMutex);
				}
			}
			writerExit(&writersMutex);

			/************** cmdL - account VIP ****************/
		} else if (args[0] == cmdL) {
			readerEnter(&readersMutex, &writersMutex, &mapReadersNum);
			//check account exits
			if (accountsMap.find(accountNum) == accountsMap.end()) {
				//doesn't exits, enter log
				pthread_mutex_lock(&logMutex);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - password for account id "
						<< accountNum << " is incorrect" << endl;
				sleep(1);
				//exits log
				pthread_mutex_unlock(&logMutex);
			} else {
				//account exists, enter writer
				writerEnter(
						&accountsMap.find(accountNum)->second.accountWritersMutex);
				Account& curAcc = accountsMap.at(accountNum);
				//check correct password
				if (password != curAcc.getPass()) {
					pthread_mutex_lock(&logMutex);
					logWriter << "Error " << atm->getSerial()
							<< ": Your transaction failed - password for account id "
							<< accountNum << " is incorrect" << endl;
					sleep(1);
					pthread_mutex_unlock(&logMutex);
				} else {
					//password was correct
					curAcc.setVIP();
					sleep(1);
				}
			}
			writerExit(
					&accountsMap.find(accountNum)->second.accountWritersMutex);
			readerExit(&readersMutex, &writersMutex, &mapReadersNum);

			/************** cmdD - Deposit ****************/
		} else if (args[0] == cmdD) {
			readerEnter(&readersMutex, &writersMutex, &mapReadersNum);
			if (accountsMap.find(accountNum) == accountsMap.end()) {
				//account doesn't exist
				pthread_mutex_lock(&logMutex);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNum << " does not exist" << endl;
				sleep(1);
				pthread_mutex_unlock(&logMutex);
			} else {
				//account exists, enter account write
				writerEnter(
						&accountsMap.find(accountNum)->second.accountWritersMutex);
				Account& curAcc = accountsMap.at(accountNum);
				//check correct password
				if (password != curAcc.getPass()) {
					pthread_mutex_lock(&logMutex);
					logWriter << "Error " << atm->getSerial()
							<< ": Your transaction failed - password for account id "
							<< accountNum << " is incorrect" << endl;
					sleep(1);
					pthread_mutex_unlock(&logMutex);
				} else {
					//password is correct
					int amount = stoi(args[3]);
					//negative amount
					if (amount < 0) {
						pthread_mutex_lock(&logMutex);
						logWriter << "Error " << atm->getSerial()
								<< "Cannot deposit a negative amount" << endl;
						sleep(1);
						pthread_mutex_unlock(&logMutex);
					} else {
						// add amount to account
						curAcc.setDeposit(amount);
						int newBalance = curAcc.getBalance();
						pthread_mutex_lock(&logMutex);
						logWriter << atm->getSerial() << ": Account "
								<< accountNum << " new balance is "
								<< newBalance << " after amount " << amount
								<< " was deposited" << endl;
						sleep(1);
						pthread_mutex_unlock(&logMutex);
					}
				}
				writerExit(
						&accountsMap.find(accountNum)->second.accountWritersMutex);
			}
			readerExit(&readersMutex, &writersMutex, &mapReadersNum);

			/************** cmdW - Withdraw ****************/
		} else if (args[0] == cmdW) {
			//reader enters
			readerEnter(&readersMutex, &writersMutex, &mapReadersNum);
			if (accountsMap.find(accountNum) == accountsMap.end()) {
				//account doesn't exist
				pthread_mutex_lock(&logMutex);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNum << " does not exist" << endl;
				sleep(1);
				pthread_mutex_unlock(&logMutex);
			} else {
				//account exists, writer enters
				writerEnter(
						&accountsMap.find(accountNum)->second.accountWritersMutex);
				Account& curAcc = accountsMap.at(accountNum);
				//check correct password
				if (password != curAcc.getPass()) {
					pthread_mutex_lock(&logMutex);
					logWriter << "Error " << atm->getSerial()
							<< ": Your transaction failed - password for account id "
							<< accountNum << " is incorrect" << endl;
					sleep(1);
					pthread_mutex_unlock(&logMutex);
				} else {
					//password is correct
					int amount = stoi(args[3]);
					if (amount < 0) {
						//negative amount
						pthread_mutex_lock(&logMutex);
						logWriter << "Error " << atm->getSerial()
								<< ": Cannot withdraw a negative amount"
								<< endl;
						sleep(1);
						pthread_mutex_unlock(&logMutex);
					} else {
						// withdraw amount from account
						if (curAcc.setWithdrawal(amount) == true) {
							int newBalance = curAcc.getBalance();
							pthread_mutex_lock(&logMutex);
							logWriter << atm->getSerial() << ": Account "
									<< accountNum << " new balance is "
									<< newBalance << " after amount " << amount
									<< " was withdrew" << endl;
							sleep(1);
							pthread_mutex_unlock(&logMutex);
						} else {
							//balance is lower than amount to be withdrawn
							pthread_mutex_lock(&logMutex);
							logWriter << "Error " << atm->getSerial()
									<< ": Your transaction failed - account id "
									<< accountNum << " balance is lower than "
									<< amount << endl;
							sleep(1);
							pthread_mutex_unlock(&logMutex);
						}
					}
				}
				writerExit(
						&accountsMap.find(accountNum)->second.accountWritersMutex);
			}
			readerExit(&readersMutex, &writersMutex, &mapReadersNum);

			/************** cmdB - Get Balance ****************/
		} else if (args[0] == cmdB) {
			readerEnter(&readersMutex, &writersMutex, &mapReadersNum);
			if (accountsMap.find(accountNum) == accountsMap.end()) {
				//account doesn't exist
				pthread_mutex_lock(&logMutex);
				logWriter << "Error " << atm->getSerial()
						<< ": Your transaction failed - account id "
						<< accountNum << " does not exist" << endl;
				sleep(1);
				pthread_mutex_unlock(&logMutex);
			} else {
				//account exists, checking password and enter account read
				readerEnter(
						&accountsMap.find(accountNum)->second.accountReadersMutex,
						&accountsMap.find(accountNum)->second.accountWritersMutex,
						&accountsMap.find(accountNum)->second.numOfReaders);
				//copy by value (no need to update)
				Account curAcc = (accountsMap.find(accountNum))->second;
				if (curAcc.getPass() != password) {
					//wrong password
					pthread_mutex_lock(&logMutex);
					logWriter << "Error " << atm->getSerial()
							<< ": Your transaction failed - password for account id "
							<< accountNum << " is incorrect" << endl;
					sleep(1);
					pthread_mutex_unlock(&logMutex);
				} else {
					//everything good
					int curBalance = curAcc.getBalance();
					pthread_mutex_lock(&logMutex);
					logWriter << atm->getSerial() << ": Account " << accountNum
							<< " balance is " << curBalance << endl;
					sleep(1);
					pthread_mutex_unlock(&logMutex);
				}
				readerExit(
						&accountsMap.find(accountNum)->second.accountReadersMutex,
						&accountsMap.find(accountNum)->second.accountWritersMutex,
						&accountsMap.find(accountNum)->second.numOfReaders);
			}
			readerExit(&readersMutex, &writersMutex, &mapReadersNum);

			/************** cmdT - Make Transfer ****************/
		} else if (args[0] == cmdT) {
			string accountNumDest = args[3];
			string transAmountStr = args[4];
			int transAmount = stoi(transAmountStr);
			if (accountNumDest != accountNum) {
				//acc src and dest are different, enter reader and writer src
				readerEnter(&readersMutex, &writersMutex, &mapReadersNum);
				if (accountsMap.find(accountNum) == accountsMap.end()) {
					//src account doesn't exist
					pthread_mutex_lock(&logMutex);
					logWriter << "Error " << atm->getSerial()
							<< ": Your transaction failed - account id "
							<< accountNum << " does not exist" << endl;
					sleep(1);
					pthread_mutex_unlock(&logMutex);
				} else {
					//src account exists, enter write and checking password
					writerEnter(
							&accountsMap.find(accountNum)->second.accountWritersMutex);
					Account& srcAcc = (accountsMap.find(accountNum))->second;
					if (srcAcc.getPass() != password) {
						//wrong password
						pthread_mutex_lock(&logMutex);
						logWriter << "Error " << atm->getSerial()
								<< ": Your transaction failed - password for account id "
								<< accountNum << " is incorrect" << endl;
						sleep(1);
						pthread_mutex_unlock(&logMutex);
					} else if (accountsMap.find(accountNumDest)
							== accountsMap.end()) {
						//src password valid, check dst account exists
						//account doesn't exist
						pthread_mutex_lock(&logMutex);
						logWriter << "Error " << atm->getSerial()
								<< ": Your transaction failed - account id "
								<< accountNumDest << " does not exist" << endl;
						sleep(1);
						pthread_mutex_unlock(&logMutex);
					} else {
						//dst account exists, enter write
						writerEnter(
								&accountsMap.find(accountNumDest)->second.accountWritersMutex);
						int balance = srcAcc.getBalance();
						int newBalance = balance - transAmount;
						if (newBalance < 0) {
							pthread_mutex_lock(&logMutex);
							logWriter << "Error " << atm->getSerial()
									<< ": Your transaction failed - account id "
									<< accountNum << " balance is lower than "
									<< transAmount << endl;
							sleep(1);
							pthread_mutex_unlock(&logMutex);
						} else {
							//everything good
							accountsMap.find(accountNum)->second.setBalance(
									newBalance);
							Account& dstAccount = (accountsMap.find(
									accountNumDest))->second;
							int dstNewBalance = dstAccount.getBalance();
							dstNewBalance += transAmount;
							(accountsMap.find(accountNumDest))->second.setBalance(
									dstNewBalance);
							pthread_mutex_lock(&logMutex);
							logWriter << atm->getSerial() << ": Transfer "
									<< transAmount << " from account "
									<< accountNum << " to account "
									<< accountNumDest
									<< " new account balance is " << newBalance
									<< " new target account balance is "
									<< dstNewBalance << endl;
							sleep(1);
							pthread_mutex_unlock(&logMutex);
						}
						//writer exit dst account
						writerExit(
								&accountsMap.find(accountNumDest)->second.accountWritersMutex);
					}
					//writer exit src acc
					writerExit(
							&accountsMap.find(accountNum)->second.accountWritersMutex);
				}
				//reader exit map
				readerExit(&readersMutex, &writersMutex, &mapReadersNum);
			}

			/************** Unknown Command ****************/
		} else {
			pthread_mutex_lock(&logMutex);
			logWriter << "Error " << atm->getSerial()
					<< ": Invalid Command Name!" << endl;
			sleep(1);
			pthread_mutex_unlock(&logMutex);
		}
		delete[] cmd;
	}
	logWriter.close();
	inputReader.close();
	return NULL;
}
