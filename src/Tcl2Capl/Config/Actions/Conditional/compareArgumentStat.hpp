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

    void toAction(UserProcedureRule::ConditionalActions::Type& conditional)override{}
};

#endif // CONFIG_CONDITIONALS_COMPAREARGUMENTSTAT_HPP
