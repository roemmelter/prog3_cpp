/*
  author: Erik Roemmelt
  module: checking account application entrypoint
*/
#include "checking.h"
#include <iostream>
using namespace std;

double test_checking_account(double giro_limit) {
  cout << "Test with giro limit: " << giro_limit << endl;
  
  CheckingAccount bank = CheckingAccount(giro_limit);
  for (int i = 0; i < 5; i++) {
    bank.deposit(100);
  }
  for (int i = 0; i < 5; i++) {
    bank.withdraw(150);
  }
  double leftover = bank.balance();
  
  cout << "Balance after test: " << leftover << endl;
  return leftover;
}

int main(int argc, char *argv[])
{
  test_checking_account(0);
  cout << endl;
  double test_result = test_checking_account(100);
  
  return(test_result);
}

