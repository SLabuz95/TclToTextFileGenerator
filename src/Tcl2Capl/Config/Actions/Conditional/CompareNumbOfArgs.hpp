#ifndef COMPARENUMBOFARGS_HPP
#define COMPARENUMBOFARGS_HPP

<<<<<<< HEAD
#include"External/Factory/products.hpp"
=======
#include"External/Factory/factory.hpp"
>>>>>>> refs/remotes/TclToTextGenerator/dev
#include"Tcl2Capl/Config/Actions/Conditional/definition.hpp"

template<>
template<>
<<<<<<< HEAD
struct ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Properties
: public ConditionalsProductDefinition::Definition
{
protected:
    //IntegerParam numbOfArgs;
=======
struct ConditionalsFactory::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Properties{
>>>>>>> refs/remotes/TclToTextGenerator/dev

};

template<>
template<>
<<<<<<< HEAD
class ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Methods
: protected ConditionalsProducts::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Properties
{
=======
class ConditionalsFactory::ImplementationData<ConditionalsTypes::CompareNumbOfArguments>::Methods{
>>>>>>> refs/remotes/TclToTextGenerator/dev

};

template<>
template<>
<<<<<<< HEAD
class ConditionalsProducts::InterfaceData<ConditionalsTypes::CompareNumbOfArguments>::Methods
: protected ConditionalsProducts::Implementation<ConditionalsTypes::CompareNumbOfArguments>
{
=======
class ConditionalsFactory::InterfaceData<ConditionalsTypes::CompareNumbOfArguments>::Methods{
>>>>>>> refs/remotes/TclToTextGenerator/dev

};


#endif // COMPARENUMBOFARGS_HPP
