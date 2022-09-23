#ifndef ADDPREDEFINITION_HPP
#define ADDPREDEFINITION_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::AddPredefinition>::Properties
: public ExecutablesProductDefinition::Definition
{
    using FormattedString = ParametersFactory::Product<Parameters::FormattedString>;
    using PredefinitionsController = Tcl::Interpreter::PredefinitionsController;
    using Scope = PredefinitionsController::Scope;
public:
    Scope scope_;
    FormattedString inputFormattedString_;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::AddPredefinition>::Methods
: public ExecutablesProducts::ImplementationData<ExecutablesTypes::AddPredefinition>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::AddPredefinition>::Methods
: public ExecutablesProducts::Implementation<ExecutablesTypes::AddPredefinition>
{
public:
    void setScope(Scope scope){scope_ = scope;}
    Scope scope()const{return scope_;}
    FormattedString& inputFormattedString(){return inputFormattedString_;}
    void toAction(UserProcedureRule::ExecutableActions::Type& conditional)override{
        using Action = UserProcedureRule::ExecutableActions::Type;
        using Parameters = UserProcedureRule::ExecutableActions::Type::Parameters;

        Parameters formattedStringParameters;
        inputFormattedString().toActionParameters(formattedStringParameters);
        conditional = Action(type(),
                             Parameters(/*
                                 QStringList{QString::number(PredefinitionsController::cast_scope(scope()))}
                                 << formattedStringParameters*/
                                 ));
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{
        xmlWriter.writeStartElement("executableAction");
        xmlWriter.writeAttribute("type", UserProcedure::Action::toStr_executable(type()));
        // Scope
        xmlWriter.writeEmptyElement("param"); // Param?
        xmlWriter.writeAttribute("type", "0"); // For compatibility with future implementation
        xmlWriter.writeAttribute("value", PredefinitionsController::toStr(scope())); // For compatibility with future implementation
        // inputFormattedString_
        inputFormattedString_.toXmlContent(xmlWriter);
        xmlWriter.writeEndElement();
    }
};


#endif // ADDPREDEFINITION_HPP
