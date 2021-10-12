#ifndef ADDFUNCTIONDEFINITION_HPP
#define ADDFUNCTIONDEFINITION_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::AddFunctionDefinition>::Properties
: protected ExecutablesProductDefinition::Definition
{

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::AddFunctionDefinition>::Methods
: protected ExecutablesProducts::ImplementationData<ExecutablesTypes::AddFunctionDefinition>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::AddFunctionDefinition>::Methods
: protected ExecutablesProducts::Implementation<ExecutablesTypes::AddFunctionDefinition>
{

};


#endif // ADDFUNCTIONDEFINITION_HPP
