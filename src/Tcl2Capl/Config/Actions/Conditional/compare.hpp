#ifndef CONFIG_CONDITIONALS_COMPARE_HPP
#define CONFIG_CONDITIONALS_COMPARE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::Compare>::Properties
: public ConditionalsProductDefinition::Definition
{
public:
    using FormattedString = ParametersFactory::Product<Parameters::FormattedString>;
protected:
     FormattedString inputFormattedString_;
     QStringList stringsToCompare_;

};

template<>
template<>
class ConditionalsProducts::ImplementationData<ConditionalsTypes::Compare>::Methods
: public ConditionalsProducts::ImplementationData<ConditionalsTypes::Compare>::Properties
{
};

template<>
template<>
class ConditionalsProducts::InterfaceData<ConditionalsTypes::Compare>::Methods
: public ConditionalsProducts::Implementation<ConditionalsTypes::Compare>
{
public:
    QStringList& stringsToCompare(){return stringsToCompare_;}
    FormattedString& inputFormattedString(){return inputFormattedString_;}
    void toAction(UserProcedureRule::ConditionalActions::Type& conditional)override{
        using Action = UserProcedureRule::ConditionalActions::Type;
        using Parameters = UserProcedureRule::ConditionalActions::Type::Parameters;
        Parameters formattedStringParameters;
        inputFormattedString().toActionParameters(formattedStringParameters);
        conditional = Action(type(),
                                            Parameters({QString::number(stringsToCompare_.size())})
                                                <<  stringsToCompare_
                                                << QStringList{QString::number(formattedStringParameters.size())}
                                                << formattedStringParameters
                                           );
    }

};


#endif // CONFIG_CONDITIONALS_COMPARE_HPP
