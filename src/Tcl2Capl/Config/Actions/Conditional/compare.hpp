#ifndef CONFIG_CONDITIONALS_COMPARE_HPP
#define CONFIG_CONDITIONALS_COMPARE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"


template<>
template<>
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::Compare>::Properties
: public ConditionalsProductDefinition::Definition
{
protected:
    //ListParam stringsToCompare;
    //ListParam inputFormattedString;

};

template<>
template<>
class ConditionalsProducts::ImplementationData<ConditionalsTypes::Compare>::Methods
: protected ConditionalsProducts::ImplementationData<ConditionalsTypes::Compare>::Properties
{
};

template<>
template<>
class ConditionalsProducts::InterfaceData<ConditionalsTypes::Compare>::Methods
: public ConditionalsProducts::Implementation<ConditionalsTypes::Compare>
{


};


#endif // CONFIG_CONDITIONALS_COMPARE_HPP
