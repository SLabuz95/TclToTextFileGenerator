#ifndef CONFIG_CONDITIONALS_COMPARENUMBOFARGS_HPP
#define CONFIG_CONDITIONALS_COMPARENUMBOFARGS_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Properties
: public ConditionalsProductDefinition::Definition
{
public:
    using ParameterType = ParametersFactory::ProductTypeEnum;
    using NumbOfArgumentsList = QStringList;
protected:
    NumbOfArgumentsList numbOfArgs_;

};

template<>
template<>
class ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Methods
: public ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Properties
{

};

template<>
template<>
class ConditionalsProducts::InterfaceData<ConditionalsTypes::CompareNumbOfArguments>::Methods
: public ConditionalsProducts::Implementation<ConditionalsTypes::CompareNumbOfArguments>
{
public:
    NumbOfArgumentsList& numbOfArgs(){return numbOfArgs_;}
    const NumbOfArgumentsList& numbOfArgs()const{return numbOfArgs_;}
    void toAction(UserProcedureRule::ConditionalActions::Type& conditional)override{
        using Action = UserProcedureRule::ConditionalActions::Type;
        using Parameters = UserProcedureRule::ConditionalActions::Type::Parameters;
        conditional = Action(type(), numbOfArgs());
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeStartElement("conditionalAction");
        xmlWriter.writeAttribute("type", UserProcedure::Action::toStr_conditional(type()));
        // numbOfArgs_
        xmlWriter.writeStartElement("param"); // List?
        xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
        for(NumbOfArgumentsList::Iterator arg = numbOfArgs().begin(); arg < numbOfArgs().end(); arg++){
            xmlWriter.writeEmptyElement("param"); // String param?
            xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
            xmlWriter.writeAttribute("value", *arg);
        }
        xmlWriter.writeEndElement(); // List? End
        xmlWriter.writeEndElement();
    }
};


#endif // CONFIG_CONDITIONALS_COMPARENUMBOFARGS_HPP
