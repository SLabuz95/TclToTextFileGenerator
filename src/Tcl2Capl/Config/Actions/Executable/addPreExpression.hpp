#ifndef ADDPREEXPRESSION_HPP
#define ADDPREEXPRESSION_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::AddPreExpression>::Properties
: public ExecutablesProductDefinition::Definition
{
    using FormattedString = ParametersFactory::Product<Parameters::FormattedString>;
protected:
     FormattedString inputFormattedString_;
    //ListParam outputParams;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::AddPreExpression>::Methods
: public ExecutablesProducts::ImplementationData<ExecutablesTypes::AddPreExpression>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::AddPreExpression>::Methods
: public ExecutablesProducts::Implementation<ExecutablesTypes::AddPreExpression>
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

#endif // ADDPREEXPRESSION_HPP
