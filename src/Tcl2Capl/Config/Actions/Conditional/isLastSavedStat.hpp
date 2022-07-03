#ifndef CONFIG_CONDITIONALS_ISLASTSAVEDSTAT_HPP
#define CONFIG_CONDITIONALS_ISLASTSAVEDSTAT_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"

template<>
template<>
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::IsLastSavedStat>::Properties
: public ConditionalsProductDefinition::Definition
{
protected:
    //IntegerParam lastSavedStat;

};

template<>
template<>
class ConditionalsProducts::ImplementationData<ConditionalsTypes::IsLastSavedStat>::Methods
: public ConditionalsProducts::ImplementationData<ConditionalsTypes::IsLastSavedStat>::Properties
{

};

template<>
template<>
class ConditionalsProducts::InterfaceData<ConditionalsTypes::IsLastSavedStat>::Methods
: public ConditionalsProducts::Implementation<ConditionalsTypes::IsLastSavedStat>
{

};

#endif // CONFIG_CONDITIONALS_ISLASTSAVEDSTAT_HPP
