#include <iostream>
#include <string>
#include <sstream>

#include "Creature.h"
#include "AxonTypes.h"
#include "Muscle.h"

using namespace EVOL_NS;

/*
Nodes {
  Const {c1 0.0}
  Time {t1}
  Add {a1}
}
Connections {
  {t1 -> a1}
  {c1 -> a1}
}
*/

class CreatureFormatBlock {
public:
    struct LeafNode {
        std::string name; //possibly blank
        std::string content; //possibly blank
        LeafNode(std::string n, std::string c) : name(n), content(c) {}
    };
    typedef std::shared_ptr<CreatureFormatBlock> BlockPtr;
private:
    std::string name; //possibly blank
    std::vector<LeafNode> leaves; //we can have several leaf nodes
    std::vector<BlockPtr> children; //we can also have nested blocks
    bool isWhitespace(char c)
    {
        return (c == ' ' or
                c == '\t' or
                c == '\n' or
                c == '\r');
    }
    std::string eatWhitespace(std::string str)
    {
        int count = 0;
        for(; isWhitespace(str[count]); ++count) {}
        return str.substr(count);
    }
    size_t findMatchingParens(std::string str, char start, char end)
    {
        int count = 1;
        for(int i = 0; i < str.length(); ++i)
        {
            if( str[i] == start ) ++count;
            if( str[i] == end ) {
              --count;
              if( count == 0 ) return i;
            }
        }
        return -1;
    }
public:
    CreatureFormatBlock() {}
    void processText(std::string text)
    {
        //check if it starts with a '{'
        text = eatWhitespace(text);
        if( text[0] != '{' ) //we have a name, save it
        {
            name = "";
            int count = 0;
            for(;!isWhitespace(text[count]) and text[count] != '{'; ++count ) {
                name += text[count];
            }
            text = text.substr(count);
        }
        //now check for content
        text = eatWhitespace(text);
        if( text[0] == '{' )
        {
            size_t close = findMatchingParens(text.substr(1), text[0], '}');
            std::string content = text.substr(1,close);
            text = text.substr(1,close+1);
            std::cout << "processed text '" << text << "'\n";
        }
    }
    friend std::ostream& operator << (std::ostream& os, CreatureFormatBlock& cfb)
    {
        os << cfb.name << " {";
        if( !cfb.leaves.empty() or !cfb.children.empty() )
            os << "\n";
        for(auto leaf : cfb.leaves)
            os << leaf.name << " {" << leaf.content << "}" << "\n";
        for(auto child : cfb.children)
            os << *child;
        os << "}";
        return os;
    }
};

class CreatureGenerator {
    int& time;
    void processNodes(std::string str, Creature& ret)
    {
        std::stringstream ss(str);
        while( !ss.eof() )
        {
            std::string type, name;
            ss >> type >> name;
            if( type == "T" )
                ret.add(name, new TimeAxon(time));
            else if( type == "C" ) {
                Axon::OutputTy value;
                ss >> value;
                ret.add(name, new ConstAxon(value));
            }
            else if( type == "A" )
                ret.add(name, new AddAxon());
            else if( type == "S" )
                ret.add(name, new SubAxon());
            //eat the rest of the line
            std::string line;
            getline(ss, line);
        }
    }
public:
    CreatureGenerator(int& t) : time(t) {}
    Creature create(std::string str)
    {
        std::stringstream ss(str);
        Creature ret;
        processNodes(str, ret);
        return ret;
    }
};

class BallEnd : public PositionableObject {
public:
    BallEnd(PositTy x, PositTy y) : PositionableObject(x,y) {}
    virtual PositionableObject::MassTy getMass() {return 1.0;}
};

int main()
{
    CreatureFormatBlock cfb;
    std::stringstream ss;
    ss << "\
Nodes {\
  Const {c1 0.0}\
  Time {t1}\
  Add {a1}\
}\
Connections {\
  {t1 -> a1}\
  {c1 -> a1}\
}";
    cfb.processText(ss.str());
    std::cout << cfb << std::endl;
    int simulationTicks = 0;
    CreatureGenerator generator(simulationTicks);
    Creature simulation;// = generator.create("T t1\nT t2\n A a1\n S s1\n");

    simulation.addAxonAsInputTo("t1", "a1");
    simulation.addAxonAsInputTo("t2", "a1");
    simulation.addAxonAsInputTo("a1", "a1");
    simulation.addAxonAsInputTo("a1", "s1");
    simulation.addAxonAsInputTo("t1", "s1");
   
    simulation.add("c1", new ConstAxon(1.0));
    //simulation.add("s2", new SubAxon());
    simulation.addAxonAsInputTo("c1", "s2");
    simulation.addAxonAsInputTo("s2", "s2");
    
    simulation.add("m1", new Muscle(1));
    simulation.addAxonAsInputTo("s2", "c1");

    PositionableObjectPtr objectA(new BallEnd(0.0,0.0));
    PositionableObjectPtr objectB(new BallEnd(9.0,9.0));
    std::dynamic_pointer_cast<Muscle>(simulation.getPartNamed("m1"))->connectEnds(objectA, objectB);

    for(;simulationTicks<10000; ++simulationTicks)
    {
        simulation.update();
        objectA->update();
        objectB->update();
        std::cout << "---------Gen " << simulationTicks << "---------\n";
        for(auto BI : simulation)
        {
            std::cout << BI.first << "(" << BI.second->getTypeAsString() << ")";
            if( AxonPtr axe = std::dynamic_pointer_cast<Axon>(BI.second) ) std::cout << " : " << axe->getOutputValue();
            if( MusclePtr musc = std::dynamic_pointer_cast<Muscle>(BI.second) ) std::cout << " : " << musc->getMuscleLength() << "L";
            std::cout << std::endl;
        }
        std::cout << "objectA : (" << objectA->getPosX() << ", " << objectA->getPosY() << ")\n";
        std::cout << "objectB : (" << objectB->getPosX() << ", " << objectB->getPosY() << ")\n";
    }
    return 0;
}
