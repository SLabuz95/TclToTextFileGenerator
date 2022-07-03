#ifndef COMPAREANDWRITE_HPP
#define COMPAREANDWRITE_HPP


#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::CompareAndWrite>::Properties
: public ExecutablesProductDefinition::Definition
{
protected:
    //ListParam outputParamaters;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::CompareAndWrite>::Methods
: public ExecutablesProducts::ImplementationData<ExecutablesTypes::CompareAndWrite>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::CompareAndWrite>::Methods
: public ExecutablesProducts::Implementation<ExecutablesTypes::CompareAndWrite>
{

};

#endif // COMPAREANDWRITE_HPP
