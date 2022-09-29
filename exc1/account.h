/*
  author: Erik Roemmelt
  module: account module provides accounting features    
*/
#pragma once

class Account
{
  public:
  Account();  // Constructor
  ~Account(); // Dectructor
  
  // Getter
  double balance();
  
  // Setter
  void deposit(double euro);
  bool withdraw(double euro);
  
  private:
  double balance_;
};
