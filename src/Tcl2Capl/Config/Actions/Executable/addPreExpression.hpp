#ifndef ADDPREEXPRESSION_HPP
#define ADDPREEXPRESSION_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::AddPreExpression>::Properties
: protected ExecutablesProductDefinition::Definition
{
protected:
    //ListParam outputParams;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::AddPreExpression>::Methods
: protected ExecutablesProducts::ImplementationData<ExecutablesTypes::AddPreExpression>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::AddPreExpression>::Methods
: protected ExecutablesProducts::Implementation<ExecutablesTypes::AddPreExpression>
{

};

#endif // ADDPREEXPRESSION_HPP
