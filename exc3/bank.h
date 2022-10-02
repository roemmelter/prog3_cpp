#pragma once

#include "account.h"

#include <map>
#include <string>

// bank interface
class Bank
{
public:

   Bank() {}
   virtual ~Bank() {}

   // open account
   // * with account number "account",
   // * password "passwd"
   // * and a reference "a" to a new account
   virtual bool open_account(int account, std::string passwd, Account *a) = 0;

   // deposit amount on account
   // * if account is existing
   virtual void deposit(int account, double amount) = 0;

   // withdraw amount from account
   // * if account is existing
   // * and if passwd matches the account password
   virtual bool withdraw(int account, std::string passwd, double amount) = 0;

   // transfer amount from one account to another one
   // * if both accounts are existing
   // * and if passwd matches the account password
   // * and if the withdrawal was successful
   virtual bool transfer(int from, int to, std::string passwd, double amount) = 0;

   // return the account balance
   // * if account is existing
   // * and if passwd matches the account password
   virtual double balance(int account, std::string passwd) = 0;

   // return the total bank balance
   // * which is the sum of all account balances
   double total_balance() { return 0; }
};
