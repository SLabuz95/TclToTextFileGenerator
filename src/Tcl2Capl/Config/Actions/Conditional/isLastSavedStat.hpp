#ifndef ISLASTSAVEDSTAT_HPP
#define ISLASTSAVEDSTAT_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"

template<>
template<>
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::IsLastSavedStat>::Properties
: protected ConditionalsProductDefinition::Definition
{
protected:
    //IntegerParam lastSavedStat;

};

template<>
template<>
class ConditionalsProducts::ImplementationData<ConditionalsTypes::IsLastSavedStat>::Methods
: protected ConditionalsProducts::ImplementationData<ConditionalsTypes::IsLastSavedStat>::Properties
{

};

template<>
template<>
class ConditionalsProducts::InterfaceData<ConditionalsTypes::IsLastSavedStat>::Methods
: protected ConditionalsProducts::Implementation<ConditionalsTypes::IsLastSavedStat>
{

};

#endif // ISLASTSAVEDSTAT_HPP
