#include "cccbank.h"

#include "checking.h"

#include <iostream>

int main(int argc, char *argv[])
{
   CCCBank ccc;

   ccc.open_account(42, "foo", new Account());
   ccc.open_account(1234, "000", new CheckingAccount(100));
   ccc.open_account(4711, "123", new CheckingAccount(1000));

   ccc.deposit(42, 1000);
   ccc.withdraw(42, "foo", 50);
   ccc.withdraw(42, "bar", 50);
   ccc.transfer(42, 4711, "foo", 100);
   ccc.transfer(1234, 4711, "000", 500);

   for (int i=1; i<=100; i++)
      ccc.open_account(i, "", new CheckingAccount(i));
   for (int i=1; i<=100; i++)
      ccc.deposit((271*(i-1)+17)%100+1, i);
   for (int i=1; i<=100; i++)
      ccc.transfer((271*(i-1)+71)%100+1, i, "", i);

   std::cout << "total bank balance: " << ccc.total_balance() << std::endl;

   return(ccc.total_balance());
}
