/*
  author: Erik Roemmelt
  module: account application entrypoint
*/

#include "account.h"
#include <iostream>

double test_account() {
  Account bank = Account();
  
  for (int i = 0; i < 5; i++) {
    bank.deposit(100.0);
  }
  
  for (int i = 0; i < 5; i++) {
    bank.withdraw(150);
  }
  
  return bank.balance();
}

int main(int argc, char *argv[])
{
  double deposits = test_account();
  std::cout << "The deposits should be 50 EUR and amounts to " << deposits << " EUR." << std::endl;
  
  return(deposits);
}

/*
  Instructions:
  + class Account
    - balance_
    + constructor()
      - set balance_ = 0
      - print balance_ confirmation
    + destructor()
    + deposit(amount)
      - add amount to balance_
    + withdraw(amount)
      - if balance_ >= amount:
        reduce balance_ by amount
        return true
    + balance()
      - return balance_
  + testAccount()
    - deposit 5 x 100 EUR, withdraw 5 x 150 EUR, display balance_
    - return balance_
  - call testAccount() in main(...)
  - assert textAccount() return value be equal 50 EUR
*/
