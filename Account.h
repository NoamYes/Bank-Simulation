/*
 * Account.h
 *
 *  Created on: May 22, 2017
 *      Author: compm
 */
#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
//cpp
#include <string>
#include <fstream>
#include <map>
#include <stack>
#include <iostream>

//100mili sec
#define ATMSLEEP 100000
#define PRINTSLEEP 500000


//#define DEBUG
#ifdef DEBUG
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define eprintf(...) do{}while(0)
#endif

using namespace std;

class Account {
public:
	Account(int accntNum, int password, int balance, std::string accountNumStr, std::string passStr);
	virtual ~Account();

	//                               ****Methods****
	// Operation  methods:
	// GetAccount

	// returns true when pass match the given passsword
	bool checkPassword(int);

	int getBalance(void);

	int getID(void);

	int getPass(void);

	void setDeposit(int);

	bool setWithdrawal(int);

	string getPassStr(void);

	string getIdStr(void);

	void setVIP();

	bool isAccountVIP();

	void setBalance(int balance);
	// Depositing
	// Withdrawal
	// GetStatus

	pthread_mutex_t accountReadersMutex;
	pthread_mutex_t accountWritersMutex;
	int _numOfReaders;
private:
	int         _accountNumber;
	string _accountNumStr;
	int         _password;
	string _passStr;
	int         _balance;
	bool isVIP = false;
};

#endif /* ACCOUNT_H_ */
