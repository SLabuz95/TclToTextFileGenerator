#ifndef COMPARE_HPP
#define COMPARE_HPP

<<<<<<< HEAD
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
=======
#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"

template<>
template<>
struct ConditionalsFactory::ImplementationData<ConditionalsTypes::Compare>::Properties{
>>>>>>> refs/remotes/TclToTextGenerator/dev

};

template<>
template<>
<<<<<<< HEAD
class ConditionalsProducts::ImplementationData<ConditionalsTypes::Compare>::Methods
: protected ConditionalsProducts::ImplementationData<ConditionalsTypes::Compare>::Properties
{
=======
class ConditionalsFactory::ImplementationData<ConditionalsTypes::Compare>::Methods{
>>>>>>> refs/remotes/TclToTextGenerator/dev

};

template<>
template<>
<<<<<<< HEAD
class ConditionalsProducts::InterfaceData<ConditionalsTypes::Compare>::Methods
: public ConditionalsProducts::Implementation<ConditionalsTypes::Compare>
{
=======
class ConditionalsFactory::InterfaceData<ConditionalsTypes::Compare>::Methods{
>>>>>>> refs/remotes/TclToTextGenerator/dev

};


#endif // COMPARE_HPP
