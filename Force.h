#ifndef _FORCE_H__
#define _FORCE_H__

#include "config.h"
#include <cmath>
#include <memory>
#include <vector>

namespace EVOL_NS {

//how much a single 'tick' is in our simulation -
//    smaller number reduces the rate of acceleration / position update
const units::time::second_t TIME_RATE(0.001_s);

typedef units::dimensionless::scalar_t ScalarTy;

template<typename Ty>
class Vector {
public:
    typedef Ty UnitTy;
private:
//a vector in our space
    UnitTy fx, fy;
public:
    Vector() : fx(0.0), fy(0.0) {}
    Vector(UnitTy _fx, UnitTy _fy) : fx(_fx), fy(_fy) {}
    UnitTy getX() const {return fx;}
    UnitTy getY() const {return fy;}
};

typedef Vector<units::force::newton_t> Force;
typedef Vector<units::acceleration::meters_per_second_squared_t> Acceleration;

//Two or more forces just sum the dimensional components
Force operator + (const Force& a, const Force& b)
{
    return Force(a.getX()+b.getX(), a.getY()+b.getY());
}

//a Vector divided by a dimensionless number is still a Vector!
template<typename Ty>
Vector<Ty> operator / (const Vector<Ty>& a, ScalarTy b)
{
    return Vector<Ty>(a.getX()/b, a.getY()/b);
}

Acceleration operator / (const Force& a, units::mass::kilogram_t b)
{
    return Acceleration(a.getX()/b, a.getY()/b);
}

//create a force in the direction of a given x/y coordinate, with vector length equal to strength
//   See: http://www.leadinglesson.com/problem-on-finding-a-vector-with-given-length-and-direction
template<class DirTy>
Force createForceInDirection(DirTy x, DirTy y, Force::UnitTy strength)
{
  DirTy distance = units::math::hypot(x,y);
  ScalarTy unitX = x / distance;
  ScalarTy unitY = y / distance;
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
    typedef units::length::meter_t PositTy;
    typedef units::velocity::meters_per_second_t VelocityTy;
    typedef units::mass::pound_t MassTy;
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
        Acceleration accel = total / getMass();
        //calculate velocity, dV = A*t
        vx += accel.getX() * TIME_RATE;
        vy += accel.getY() * TIME_RATE;
        //calculate position, dP = V*t
        posx += vx * TIME_RATE;
        posy += vy * TIME_RATE;
    }
};

PositionableObject::PositTy getDistance(PositionableObjectPtr objectA, PositionableObjectPtr objectB)
{
    return units::math::hypot(objectA->getPosX() - objectB->getPosX(), objectA->getPosY() - objectB->getPosY()); 
}

}; //namespace EVOL_NS

#endif

