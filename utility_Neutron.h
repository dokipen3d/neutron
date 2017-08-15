
#ifndef __UTILITY_Neutron_h__
#define __UTILITY_Neutron_h__

#include <vector>
#include <string>

class mySim{
public:
    std::string fragmentShader;
    bool shaderNeedsRecompile = true;
    void doSomething();
};


class myFluid{

public:

    static std::vector<mySim> simvec;
    

};



#endif