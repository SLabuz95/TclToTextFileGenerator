#ifndef WRITE_HPP
#define WRITE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::Write>::Properties
: protected ExecutablesProductDefinition::Definition
{
protected:
    //ListParam outputParamaters;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::Write>::Methods
: protected ExecutablesProducts::ImplementationData<ExecutablesTypes::Write>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::Write>::Methods
: protected ExecutablesProducts::Implementation<ExecutablesTypes::Write>
{

};


#endif // WRITE_HPP