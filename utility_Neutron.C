#include "utility_Neutron.h"
#include <iostream>

void mySim::doSomething()
{
     std::cout << "doing something" << std::endl;
}



//this is a static variable so it needs to be defined (its simply declared in the header)
//previously we could have defined in in each .C of single hcustom files but when we compile them together
//they need it to be done in one place otherwise we get duplicate symbols.
std::vector<mySim> myFluid::simvec;
