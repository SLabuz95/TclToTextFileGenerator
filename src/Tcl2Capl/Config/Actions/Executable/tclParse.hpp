#ifndef TCLPARSE_HPP
#define TCLPARSE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::TclParse>::Properties
: public ExecutablesProductDefinition::Definition
{
    using FormattedString = ParametersFactory::Product<Parameters::FormattedString>;
protected:
     FormattedString inputFormattedString_;
    //ListParam formattedStringToParse;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::TclParse>::Methods
: public ExecutablesProducts::ImplementationData<ExecutablesTypes::TclParse>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::TclParse>::Methods
: public ExecutablesProducts::Implementation<ExecutablesTypes::TclParse>
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

    }
};


#endif // TCLPARSE_HPP
