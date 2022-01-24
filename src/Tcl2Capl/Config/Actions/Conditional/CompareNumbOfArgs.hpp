#ifndef COMPARENUMBOFARGS_HPP
#define COMPARENUMBOFARGS_HPP


#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"

template<>
template<>
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Properties
: public ConditionalsProductDefinition::Definition
{
protected:
    //IntegerParam numbOfArgs;

};

template<>
template<>
class ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Methods
: protected ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Properties
{

};

template<>
template<>
class ConditionalsProducts::InterfaceData<ConditionalsTypes::CompareNumbOfArguments>::Methods
: protected ConditionalsProducts::Implementation<ConditionalsTypes::CompareNumbOfArguments>
{

};


#endif // COMPARENUMBOFARGS_HPP
