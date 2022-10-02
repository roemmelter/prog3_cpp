/*
  author: Erik Roemmelt
  module: checking account module provides additional accounting features 
*/
#pragma once

#include "account.h"

class CheckingAccount : public Account
{
public:
  CheckingAccount(double giro_limit = 0.0); // Constructor
  ~CheckingAccount();                       // Dectructor
  
  // Getter
  double get_giro();
  
  // Setter
  bool withdraw(double euro);

private:
  double giro_;
};
