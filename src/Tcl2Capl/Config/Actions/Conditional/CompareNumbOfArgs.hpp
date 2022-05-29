#ifndef COMPARENUMBOFARGS_HPP
#define COMPARENUMBOFARGS_HPP


#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"
#include"Tcl2Capl/Config/Parameters/FCT_products.hpp"

template<>
template<>
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Properties
: public ConditionalsProductDefinition::Definition
{
public:
    using ParameterType = ParametersFactory::ProductTypeEnum;
    using NumbOfArgumentsList = ParametersFactory::Product<ParameterType::List>;
protected:
    NumbOfArgumentsList numbOfArgs_;

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
public:
    NumbOfArgumentsList& numbOfArgs(){return numbOfArgs_;}
};


#endif // COMPARENUMBOFARGS_HPP
