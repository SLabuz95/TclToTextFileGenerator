#ifndef ERROR_HPP
#define ERROR_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::Error>::Properties
: public ExecutablesProductDefinition::Definition
{
    using FormattedString = ParametersFactory::Product<Parameters::FormattedString>;
protected:
     FormattedString inputFormattedString_;
};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::Error>::Methods
: public ExecutablesProducts::ImplementationData<ExecutablesTypes::Error>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::Error>::Methods
: public ExecutablesProducts::Implementation<ExecutablesTypes::Error>
{
public:
    FormattedString& inputFormattedString(){return inputFormattedString_;}
    void toAction(UserProcedureRule::ExecutableActions::Type& executable)override{
        using Action = UserProcedureRule::ExecutableActions::Type;
        using Parameters = UserProcedureRule::ExecutableActions::Type::Parameters;
        Parameters formattedStringParameters;
        inputFormattedString().toActionParameters(formattedStringParameters);
        executable = Action(type(),formattedStringParameters);
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeStartElement("executableAction");
        xmlWriter.writeAttribute("type", UserProcedure::Action::toStr_executable(type()));
        inputFormattedString_.toXmlContent(xmlWriter);
        xmlWriter.writeEndElement();
    }
};

#endif // ERROR_HPP
