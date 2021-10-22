#ifndef TCLPARSE_HPP
#define TCLPARSE_HPP

#include"External/Factory/products.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct ExecutablesProducts::ImplementationData<ExecutablesTypes::TclParse>::Properties
: protected ExecutablesProductDefinition::Definition
{
protected:
    //ListParam formattedStringToParse;

};

template<>
template<>
class ExecutablesProducts::ImplementationData<ExecutablesTypes::TclParse>::Methods
: protected ExecutablesProducts::ImplementationData<ExecutablesTypes::TclParse>::Properties
{

};

template<>
template<>
class ExecutablesProducts::InterfaceData<ExecutablesTypes::TclParse>::Methods
: protected ExecutablesProducts::Implementation<ExecutablesTypes::TclParse>
{

};


#endif // TCLPARSE_HPP
