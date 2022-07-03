#ifndef ERROR_HPP
#define ERROR_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::Error>::Properties
: public ExecutablesProductDefinition::Definition
{
protected:
    //ListParam outputParams;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::Error>::Methods
: public ExecutablesProducts::ImplementationData<ExecutablesTypes::Error>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::Error>::Methods
: public ExecutablesProducts::Implementation<ExecutablesTypes::Error>
{

};

#endif // ERROR_HPP
