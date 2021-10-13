#ifndef CHANGELASTSAVEDSTAT_HPP
#define CHANGELASTSAVEDSTAT_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::ChangeLastSavedStat>::Properties
: protected ExecutablesProductDefinition::Definition
{
protected:
    //IntegerParam newStat;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::ChangeLastSavedStat>::Methods
: protected ExecutablesProducts::ImplementationData<ExecutablesTypes::ChangeLastSavedStat>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::ChangeLastSavedStat>::Methods
: protected ExecutablesProducts::Implementation<ExecutablesTypes::ChangeLastSavedStat>
{

};

#endif // CHANGELASTSAVEDSTAT_HPP
