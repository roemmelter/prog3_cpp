/*
  author: Erik Roemmelt
  module: implements the accounting class
*/

#include "account.h"
#include <iostream>

// Constructor
Account::Account() {
  balance_ = 0.0;
  std::cout << "The current balance amounts to " << balance_ << " EUR" << std::endl;
}

// Destructor
Account::~Account() {}

// Getter
double Account::balance() {
  return balance_;
}

// Setter
void Account::deposit(double euro) {
  if (euro > 0.0) {
    balance_ += euro;
  } else {
    std::cout << "Operation invalid: Please enter an amount greater than nothing." << std::endl;
  }
}

bool Account::withdraw(double euro) {
  if (euro < 0.0) {
    std::cout << "Operation invalid: We appreciate your donation very much, " 
    << "unfortunately we are not allowed to accept it in this way." << std::endl;
    return false;
  }
  if (balance_ >= euro) {
    balance_ -= euro;
    return true;
  } 
  return false;
}


