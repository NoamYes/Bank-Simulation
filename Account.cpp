/*
 * Atm.cpp
 *
 *  Created on: Dec 17, 2018
 *      Author: os
 */

#include "Account.h"

using namespace std;



Account::Account(string accountNum, string password, int balance) :
		accountNumber(accountNum), password(password), balance(balance),
		isVIP(false) {};


int Account::getBalance(void) {
	return balance;
}

string Account::getID(void) {
	return accountNumber;
}

string Account::getPass(void) {
	return password;
}

void Account::setBalance(int newBalance) {
	balance = newBalance;
	return;
}

void Account::setDeposit(int amount) {
	balance += amount;
	return;
}

bool Account::setWithdrawal(int amount) {
	if (balance >= amount) {
		balance -= amount;
		return true;
	} else {
		return false;
	}

}

bool Account::checkPassword(string candidate) {
	if (candidate == password) {
		return true;
	}
	return false;
}

void Account::setVIP() {
	this->isVIP = true;
}

bool Account::isAccountVIP() {
	return this->isVIP;
}
