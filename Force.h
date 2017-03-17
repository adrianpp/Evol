#ifndef _FORCE_H__
#define _FORCE_H__

#include "config.h"
#include <cmath>
#include <memory>
#include <vector>

namespace EVOL_NS {

//larger number reduces the rate of acceleration / position update
const float TIME_RATE = 50;

class Force {
public:
    typedef float ForceTy;
private:
//force is a vector in our space
    ForceTy fx, fy;
public:
    Force() : fx(0.0), fy(0.0) {}
    Force(ForceTy _fx, ForceTy _fy) : fx(_fx), fy(_fy) {}
    ForceTy getX() const {return fx;}
    ForceTy getY() const {return fy;}
};

//Two or more forces just sum the dimensional components
Force operator + (const Force& a, const Force& b)
{
    return Force(a.getX()+b.getX(), a.getY()+b.getY());
}

//a force divided by a dimensionless number is still a force!
Force operator / (const Force& a, Force::ForceTy b)
{
    return Force(a.getX()/b, a.getY()/b);
}

//create a force in the direction of a given x/y coordinate, with vector length equal to strength
//   See: http://www.leadinglesson.com/problem-on-finding-a-vector-with-given-length-and-direction
Force createForceInDirection(Force::ForceTy x, Force::ForceTy y, Force::ForceTy strength)
{
  Force::ForceTy distance = std::hypot(x,y);
  Force::ForceTy unitX = x / distance;
  Force::ForceTy unitY = y / distance;
  return Force(unitX * strength, unitY * strength);
}

class PositionableObject;

typedef std::shared_ptr<PositionableObject> PositionableObjectPtr;

//this class represents a source of force
class ForceSource {
public:
    virtual Force getForce(PositionableObject*)=0;
};

typedef std::shared_ptr<ForceSource> ForceSourcePtr;

//this class represents something that exists in space
//     and can have forces applied to it
class PositionableObject {
public:
    typedef float PositTy;
    typedef float VelocityTy;
    typedef float MassTy;
private:
    PositTy posx, posy;
    VelocityTy vx, vy;
    std::vector<ForceSourcePtr> forceSources;
protected:
    virtual MassTy getMass()=0;
public:
    PositionableObject(PositTy x, PositTy y) : posx(x), posy(y), vx(0.0), vy(0.0) {}
    PositTy getPosX(){return posx;}
    PositTy getPosY(){return posy;}
    void addForceSource(ForceSourcePtr fs) {forceSources.push_back(fs);}
    void update()
    {
        //sum forces acting on us
        Force total;
        for( auto fsource : forceSources )
        {
            total = total + fsource->getForce(this);
        }
        //calculate acceleration, F=M*A
        Force accel = total / getMass();
        //calculate velocity, dV = A*t
        vx += accel.getX() / TIME_RATE;
        vy += accel.getY() / TIME_RATE;
        //calculate position, dP = V*t
        posx += vx / TIME_RATE;
        posy += vy / TIME_RATE;
    }
};

PositionableObject::PositTy getDistance(PositionableObjectPtr objectA, PositionableObjectPtr objectB)
{
    return std::hypot(objectA->getPosX() - objectB->getPosX(), objectA->getPosY() - objectB->getPosY()); 
}

}; //namespace EVOL_NS

#endif

