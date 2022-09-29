/*
  author: Erik Roemmelt
  module: implements the checking account class
*/
#include "checking.h"
#include <iostream>

// Constructor
CheckingAccount::CheckingAccount(double giro_limit) : Account() {
  giro_ = giro_limit;
}

// Destructor
CheckingAccount::~CheckingAccount() {}

// Getter
double CheckingAccount::get_giro() {
  return giro_;
}

// Setter
bool CheckingAccount::withdraw(double euro) {
  if (euro < 0.0) {
    std::cout << "Operation invalid: We appreciate your donation very much, " 
    << "unfortunately we are not allowed to accept it in this way." << std::endl;
    return false;
  }
  if ((balance_ + giro_ - euro) >= 0) {
    balance_ -= euro;
    return true;
  } 
  return false;
}
