#ifndef _BODY_PART_H__
#define _BODY_PART_H__

#include "config.h"
#include <string>
#include <memory>

namespace EVOL_NS {

//BodyPart uses two stage updating, so that ordering of inputs
//  does not matter; first, update() is called on all parts, then
//  commit() is called; this allows a part to update() internally,
//  but not to change the external value shown, and then commit that
//  change once all parts have updated
class BodyPart {
public:
//all calcuations are done in update
    virtual void update()=0;
//commit takes the changes found in update and locks them in
    virtual void commit()=0;
//every class has a type, specified as a string
    virtual std::string getTypeAsString()=0;
};

typedef std::shared_ptr<BodyPart> BodyPartPtr;

}; //namespace EVOL_NS

#endif

