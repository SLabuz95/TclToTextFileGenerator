#ifndef ADDUSERINTERACTION_HPP
#define ADDUSERINTERACTION_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"


template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::AddUserInteraction>::Properties
: protected ExecutablesProductDefinition::Definition
{
protected:
    //ListParam outputParams;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::AddUserInteraction>::Methods
: protected ExecutablesProducts::ImplementationData<ExecutablesTypes::AddUserInteraction>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::AddUserInteraction>::Methods
: protected ExecutablesProducts::Implementation<ExecutablesTypes::AddUserInteraction>
{

};

#endif // ADDUSERINTERACTION_HPP
