#ifndef _AXON_H__
#define _AXON_H__

#include "config.h"
#include <memory>
#include <vector>
#include "BodyPart.h"

namespace EVOL_NS {

class Axon;

typedef std::shared_ptr<Axon> AxonPtr;

class CanHaveAxonInputs {
    std::vector<AxonPtr> inputAxons;
public:
    CanHaveAxonInputs() {}
//accessors to the input vector
    void addAxonAsInput(AxonPtr in) { inputAxons.push_back(in); }
    decltype(inputAxons)::iterator inputBegin() { return inputAxons.begin(); }
    decltype(inputAxons)::iterator inputEnd() { return inputAxons.end(); }
};

typedef std::shared_ptr<CanHaveAxonInputs> CanHaveAxonInputsPtr;

class Axon : public BodyPart, public CanHaveAxonInputs {
public:
    typedef float OutputTy;
private:
//the old and new values are cached to allow ordering-independent updating
    OutputTy oldValue, newValue;
    virtual float calculateNewOutput()=0;
public:
    Axon() : oldValue(0.0), newValue(0.0) {}
//an output value
    OutputTy getOutputValue() { return oldValue; }
//take the other Axon's output values and use some function to create your output
    virtual void update() { newValue = calculateNewOutput(); }
//finally, commit the value we calculated
    virtual void commit() { oldValue = newValue; }
};

}; //namespace EVOL_NS

#endif

