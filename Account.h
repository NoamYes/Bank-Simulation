/*
 * Atm.cpp
 *
 *  Created on: Dec 17, 2018
 *      Author: os
 */

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <pthread.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <string>
#include <fstream>
#include <map>
#include <stack>
#include <iostream>

//100mili sec
const int atm_sleep_time = 100000;
const int print_sleep_time = 500000;

//#define DEBUG
#ifdef DEBUG
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define eprintf(...) do{}while(0)
#endif

using namespace std;

class Account {

private:
	string accountNumber;
	string password;
	int balance;
	bool isVIP = false;

public:
	/*pthread_mutex_t accountReadersMutex;
	pthread_mutex_t accountWritersMutex;
	int _numOfReaders;
	*/
	Account(string, string, int);

	~Account() = default;

	//                               ****Methods****
	// Operation  methods:
	// GetAccount

	//returns true when password matches
	bool checkPassword(string);

	int getBalance(void);

	string getID(void);

	string getPass(void);

	void setDeposit(int);

	bool setWithdrawal(int);

	void setVIP();

	bool isAccountVIP();

	void setBalance(int balance);

};

#endif /* ACCOUNT_H_ */
