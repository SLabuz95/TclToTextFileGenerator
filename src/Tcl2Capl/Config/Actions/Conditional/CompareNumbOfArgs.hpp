#ifndef CONFIG_CONDITIONALS_COMPARENUMBOFARGS_HPP
#define CONFIG_CONDITIONALS_COMPARENUMBOFARGS_HPP


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
: public ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Properties
{

};

template<>
template<>
class ConditionalsProducts::InterfaceData<ConditionalsTypes::CompareNumbOfArguments>::Methods
: public ConditionalsProducts::Implementation<ConditionalsTypes::CompareNumbOfArguments>
{
public:
    NumbOfArgumentsList& numbOfArgs(){return numbOfArgs_;}
};


#endif // CONFIG_CONDITIONALS_COMPARENUMBOFARGS_HPP
