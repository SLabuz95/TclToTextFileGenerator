#ifndef CONFIG_CONDITIONALS_COMPAREARGUMENTSTAT_HPP
#define CONFIG_CONDITIONALS_COMPAREARGUMENTSTAT_HPP

#include"External/Factory/products.hpp"

#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"

template<>
template<>
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareArgumentStat>::Properties
: public ConditionalsProductDefinition::Definition
{
protected:
    //IntegerParam lastSavedStat;
    //int numbOfArgs_;

};

template<>
template<>
class ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareArgumentStat>::Methods
: public ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareArgumentStat>::Properties
{

};

template<>
template<>
class ConditionalsProducts::InterfaceData<ConditionalsTypes::CompareArgumentStat>::Methods
: public ConditionalsProducts::Implementation<ConditionalsTypes::CompareArgumentStat>
{
public:
    //NumbOfArgumentsList& numbOfArgs(){return numbOfArgs_;}
    //const NumbOfArgumentsList& numbOfArgs()const{return numbOfArgs_;}
    void toAction(UserProcedureRule::ConditionalActions::Type& conditional)override{
        using Action = UserProcedureRule::ConditionalActions::Type;
        using Parameters = UserProcedureRule::ConditionalActions::Type::Parameters;
        conditional = Action(type(), Parameters());
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeStartElement("conditionalAction");
        xmlWriter.writeAttribute("type", UserProcedure::Action::toStr_conditional(type()));
        // numbOfArgs_
        /*xmlWriter.writeStartElement("param"); // List?
        xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
        for(NumbOfArgumentsList::Iterator arg = numbOfArgs().begin(); arg < numbOfArgs().end(); arg++){
            xmlWriter.writeEmptyElement("param"); // String param?
            xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
            xmlWriter.writeAttribute("value", *arg);
        }
        xmlWriter.writeEndElement(); // List? End*/
        xmlWriter.writeEndElement();
    }
};

#endif // CONFIG_CONDITIONALS_COMPAREARGUMENTSTAT_HPP
