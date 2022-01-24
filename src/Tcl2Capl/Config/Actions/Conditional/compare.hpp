#ifndef COMPARE_HPP
#define COMPARE_HPP

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


#endif // COMPARE_HPP
