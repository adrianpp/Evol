#ifndef _AXON_TYPES_H__
#define _AXON_TYPES_H__

#include "config.h"
#include <string>
#include "Axon.h"

namespace EVOL_NS {

//------------- Axons that don't use any inputs -------------

class ConstAxon : public Axon {
    Axon::OutputTy constVal;
    virtual Axon::OutputTy calculateNewOutput() {return constVal;}
public:
    ConstAxon(Axon::OutputTy init) : constVal(init) {}
    virtual std::string getTypeAsString() {return "ConstAxon";}
};

class TimeAxon : public Axon {
    int& simulationTicks;
    virtual Axon::OutputTy calculateNewOutput() { return simulationTicks; }
public:
    TimeAxon(int& ticks) : simulationTicks(ticks) {}
    virtual std::string getTypeAsString() {return "TimeAxon";}
};

//------------- Axons that implement normal math operations  -------------

class AddAxon : public Axon {
    virtual Axon::OutputTy calculateNewOutput()
    {
        Axon::OutputTy ret = 0;
        for(auto II = Axon::inputBegin(); II != Axon::inputEnd(); ++II)
        {
            ret += (*II)->getOutputValue();
        }
        return ret;
    }
public:
    virtual std::string getTypeAsString() {return "AddAxon";}
};

class SubAxon : public Axon {
    virtual Axon::OutputTy calculateNewOutput()
    {
        Axon::OutputTy ret = 0;
        for(auto II = Axon::inputBegin(); II != Axon::inputEnd(); ++II)
        {
            //first input is the base value, all others subtracted from it
            if( II == Axon::inputBegin() )
                ret = (*II)->getOutputValue();
            else
                ret -= (*II)->getOutputValue();
        }
        return ret;
    }
public:
    virtual std::string getTypeAsString() {return "SubAxon";}
};


}; //namespace EVOL_NS

#endif

