#ifndef _CREATURE_H__
#define _CREATURE_H__

#include "config.h"
#include <string>
#include <map>
#include "Axon.h"
#include "BodyPart.h"

namespace EVOL_NS {

class Creature {
    std::map<std::string,BodyPartPtr> parts;
public:
    void add(std::string name, BodyPartPtr part) {parts[name] = part;}
    void add(std::string name, BodyPart* part) {add(name, BodyPartPtr(part));}
//accessors
    decltype(parts)::iterator begin() {return parts.begin();}
    decltype(parts)::iterator end() {return parts.end();}
    BodyPartPtr getPartNamed(std::string name)
    {
        auto result = parts.find(name);
        if( result == end() )
            return BodyPartPtr();
        else
            return result->second;
    }
    AxonPtr getAxonNamed(std::string name)
    {
        return std::dynamic_pointer_cast<Axon>(getPartNamed(name));
    }
//connections
    bool addAxonAsInputTo(std::string input, std::string base)
    {
        AxonPtr inAxon = getAxonNamed(input);
        CanHaveAxonInputsPtr basePart = std::dynamic_pointer_cast<CanHaveAxonInputs>(getPartNamed(base));
        if( !inAxon or !basePart ) return false; //we failed to make connnection
        basePart->addAxonAsInput(inAxon);
        return true;
    }
//state update
    void update()
    {
        for(auto BI : parts)
            BI.second->update();
        for(auto BI : parts)
            BI.second->commit();
    }
};

}; //namespace EVOL_NS

#endif

