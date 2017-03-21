#ifndef _MUSCLE_H__
#define _MUSCLE_H__

#include "config.h"
#include <memory>
#include "Axon.h"
#include "BodyPart.h"
#include "Force.h"

//spring constants aren't a first class type, so add them
namespace units {
UNIT_ADD(spring, kilogram_per_second_squared, kilograms_per_second_squared, kgps_sq, compound_unit<mass::kilogram, inverse<squared<time::seconds>>>)
};

namespace EVOL_NS {

class Muscle : public BodyPart, public CanHaveAxonInputs, public ForceSource {
public:
    typedef PositionableObject::PositTy LengthTy;
//spring constant is measured in Newtons/meter, or kilograms/second^2
    typedef units::spring::kilogram_per_second_squared_t RigidityTy;
private:
    LengthTy desiredLength;
//the rigidity of a muscle is how hard it pushes/pulls on the nodes it is connected to
    RigidityTy rigidity;
//the two ends of the muscle
    PositionableObjectPtr objectA, objectB;
//forces applied to objectA and objectB
    Force forceA, forceB;
public:
    Muscle(RigidityTy rigid) : desiredLength(1.0), rigidity(rigid) {}
//connect the ends of the muscle
    void connectEnds(PositionableObjectPtr a, PositionableObjectPtr b)
    {
        objectA = a;
        objectB = b;
        ForceSourcePtr thisptr(this);
        objectA->addForceSource(thisptr);
        objectB->addForceSource(thisptr);
    }
//muscle length is distance between points
    LengthTy getMuscleLength() { return getDistance(objectA, objectB); }
    virtual void update()
    {
        //scale desiredLength based on the input axon (only first is considered)
        Axon::OutputTy ret = 1.0;
        if( inputBegin() != inputEnd() )
        {
            ret *= (*inputBegin())->getOutputValue();
        }
        if( ret < 0.5 ) ret = 0.5;
        if( ret > 1.5 ) ret = 1.5;
        //scale the actual length by the scaling to get desired length
        //TODO: there should be some baseline length that we are working off of!
        desiredLength = 1.0_m * ret;
    }
//finally, commit the value we calculated
    virtual void commit()
    {
        //update forces based on our curLength/desiredLength and rigidity
        LengthTy difference = desiredLength - getMuscleLength();
        //the force put on objectA will be in the direction of objectB, with a vector length proportional to half the difference times our rigidity
        forceA = createForceInDirection(objectA->getPosX() - objectB->getPosX(), objectA->getPosY() - objectB->getPosY(), rigidity * difference / 2.0);
        //the force put on objectB will be in the direction of objectA, with a vector length proportional to half the difference times our rigidity
        forceB = createForceInDirection(objectB->getPosX() - objectA->getPosX(), objectB->getPosY() - objectA->getPosY(), rigidity * difference / 2.0);
    }
//needed for BodyPart
    virtual std::string getTypeAsString() {return "Muscle";}
//needed for ForceSource
    virtual Force getForce(PositionableObject* p)
    {
        if( p == objectA.get() )
        {
            return forceA;
        }
        else if( p == objectB.get() )
        {
            return forceB;
        }
        else
        {
            return Force();
        }
    }
};

typedef std::shared_ptr<Muscle> MusclePtr;

}; //namespace EVOL_NS

#endif

