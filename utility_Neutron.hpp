
#ifndef __UTILITY_Neutron_h__
#define __UTILITY_Neutron_h__


class mySim{
public:
    void doSomething(){
            std::cout << "doing something" << std::endl;
    }
};


class myFluid{

public:
    myFluid(){}

    static std::vector<mySim> simvec;

};



#endif