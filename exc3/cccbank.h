#pragma once

#include "bank.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;

// account with associated protection password
class ProtectedAccount
{
public:

    ProtectedAccount(Account *a, string p)
        : account(a), passwd(p) {}

    Account *account; // public -> no getter required
    string passwd; // public -> no getter required

};

// CCC-Bank
class CCCBank: public Bank
{
public:

    CCCBank() : Bank() {}

    ~CCCBank() {}

    // open account
    // * with account number "account",
    // * password "passwd"
    // * and a reference "a" to a new account
    // * returns true if the account could be opened successfully
    bool open_account(int account_nr, string passwd, Account *a) {
        ProtectedAccount new_account = ProtectedAccount(a, passwd);
        accounts_.insert(pair<int, ProtectedAccount>(account_nr, new_account));
        cout << "Account (nr: "<< account_nr <<", passwd: "<< passwd <<") created with a balance of EUR: "<< a->balance() << std::endl;
        
        if (account_nr == accounts_.find(account_nr)->first) return true;
        
        return false;
    }

    // deposit amount on account
    // * if account is existing
    void deposit(int account_nr, double amount) {
        map<int, ProtectedAccount>::iterator it;
        
        it = accounts_.find(account_nr);
        if (account_nr == it->first) {
            (it->second).account->deposit(amount);
        } else {
          cout << "# deposit failed: no matching account = " << account_nr << endl; 
        }
    }

    // withdraw amount from account
    // * if account is existing
    // * and if passwd matches the account password
    // * returns true if the withdrawal was successful
    bool withdraw(int account_nr, string passwd, double amount) {
        map<int, ProtectedAccount>::iterator it;
        
        it = accounts_.find(account_nr);
        if (account_nr == it->first && passwd.compare((it->second).passwd) == 0) {
            (it->second).account->withdraw(amount);
            return true;
        } else {
          cout << "# withdraw failed: no matching account = " << account_nr << " with passwd = " << passwd << endl;
        }
        
        return false;
    }

    // transfer amount from one account to another one
    // * if both accounts are existing
    // * and if passwd matches the account password
    // * and if the withdrawal was successful
    // * returns true if the transfer was successful
    bool transfer(int from, int to, string passwd, double amount) {
        map<int, ProtectedAccount>::iterator it_from;
        map<int, ProtectedAccount>::iterator it_to;
        
        it_from = accounts_.find(from);
        it_to = accounts_.find(to);
        if ((from == it_from->first && to == it_to->first) && (passwd.compare((it_from->second).passwd) == 0)) {
            (it_from->second).account->withdraw(amount);
            (it_to->second).account->deposit(amount);
            cout << "# transfered amount: "<< amount <<"; from "<< from <<" -> to "<< to <<" successfully" << endl;
            return true;
        } else {
          cout << "# transfer failed: matching from->to correct= "<< (from == it_from->first && to == it_to->first) <<", pass correct= "<< passwd.compare((it_from->second).passwd) << endl;
        }
        
        return false;
    }

    // return the account balance
    // * if account is existing
    // * and if passwd matches the account password
    double balance(int account_nr, string passwd) {
        map<int, ProtectedAccount>::iterator it;
        
        it = accounts_.find(account_nr);
        if (account_nr == it->first && passwd.compare((it->second).passwd)) {
            return ((it->second).account->balance());
        } else {
          cout << "# withdraw failed: no matching account = " << account_nr << " with passwd = " << passwd << endl;
        }
        return 0;
    }

    // return the total bank balance
    // * which is the sum of all account balances
    double total_balance() { 
        //map<int, ProtectedAccount>::iterator it;
        double total = 0;
        
        for (auto it : accounts_) { 
          total += (it.second).account->balance();
        }
        return total; 
    }

protected:
    map<int, ProtectedAccount> accounts_;
};
