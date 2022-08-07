#ifndef WRITE_HPP
#define WRITE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::Write>::Properties
: public ExecutablesProductDefinition::Definition
{
    using FormattedString = ParametersFactory::Product<Parameters::FormattedString>;
protected:
     FormattedString inputFormattedString_;
    //ListParam outputParamaters;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::Write>::Methods
: public ExecutablesProducts::ImplementationData<ExecutablesTypes::Write>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::Write>::Methods
: public ExecutablesProducts::Implementation<ExecutablesTypes::Write>
{
public:
    FormattedString& inputFormattedString(){return inputFormattedString_;}
    void toAction(UserProcedureRule::ExecutableActions::Type& conditional)override{
        using Action = UserProcedureRule::ExecutableActions::Type;
        using Parameters = UserProcedureRule::ExecutableActions::Type::Parameters;
        Parameters formattedStringParameters;
        inputFormattedString().toActionParameters(formattedStringParameters);
        conditional = Action(type(),formattedStringParameters);
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeStartElement("executableAction");
        xmlWriter.writeAttribute("type", UserProcedure::Action::toStr_executable(type()));
        // inputFormattedString_
        inputFormattedString_.toXmlContent(xmlWriter);
        xmlWriter.writeEndElement();
    }
};


#endif // WRITE_HPP
