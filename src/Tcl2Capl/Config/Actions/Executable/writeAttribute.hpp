
#ifndef TCL_ACTION_WRITEATTRIBUTE_HPP
#define TCL_ACTION_WRITEATTRIBUTE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::WriteAttribute>::Properties
: public ExecutablesProductDefinition::Definition
{
    using FormattedString = ParametersFactory::Product<Parameters::FormattedString>;
protected:

    QString name_;
    FormattedString inputFormattedString_;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::WriteAttribute>::Methods
: public ExecutablesProducts::ImplementationData<ExecutablesTypes::WriteAttribute>::Properties
{
};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::WriteAttribute>::Methods
: public ExecutablesProducts::Implementation<ExecutablesTypes::WriteAttribute>
{
public:
    QString name(){return name_;}
    void setName(QString name){name_ = name;}
    FormattedString& inputFormattedString(){return inputFormattedString_;}
    void toAction(UserProcedureRule::ExecutableActions::Type& action)override{
        using Action = UserProcedureRule::ExecutableActions::Type;
        using Parameters = UserProcedureRule::ExecutableActions::Type::Parameters;
        Parameters formattedStringParameters;
        inputFormattedString().toActionParameters(formattedStringParameters);

        action = Action{type(),
                        Parameters({name_}) << formattedStringParameters
                        };
    }

    void toXmlContent(QXmlStreamWriter& xmlWriter)override{

    }
};
#endif // TCL_ACTION_WRITEATTRIBUTE_HPP

