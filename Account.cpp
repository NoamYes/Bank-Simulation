/*
 * Account.cpp
 *
 *  Created on: May 22, 2017
 *      Author: compm
 */

#include "Account.h"

using namespace std;

// Global Variables
//extern map<int, Account> accountsMap;
//extern Account  bankAccount;


Account::Account(int accntNum, int password, int balance, string accountNumStr, string passStr)
{
	 pthread_mutex_init(&accountReadersMutex, NULL);
	 pthread_mutex_init(&accountWritersMutex, NULL);
	_accountNumber = accntNum;
	_password = password;
	_balance = balance;
	_passStr = passStr;
	_accountNumStr = accountNumStr;
	_numOfReaders = 0;
}

Account::~Account()
{
	// TODO Auto-generated destructor stub
	
	//pthread_mutex_destroy(&accountReadersMutex); // hod
	//pthread_mutex_destroy(&accountWritersMutex); // hod
}

bool Account::checkPassword(int candidate)
{
	if(candidate == _password)
	{
		return true;
	}
	return false;
}

int Account::getBalance(void)
{
	return _balance;
}
int Account::getID(void)
{
	return _accountNumber;
}

int Account::getPass(void)
{
	return _password;
}

string Account::getPassStr(void)
{
	return _passStr;
}

string Account::getIdStr(void)
{
	return _accountNumStr;
}

void Account::setBalance(int balance)
{
	_balance = balance;
	return;
}
