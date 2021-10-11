#ifndef ERROR_HPP
#define ERROR_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct ExecutablesFactory::ImplementationData<ExecutablesTypes::Error>::Properties{

};

template<>
template<>
class ExecutablesFactory::ImplementationData<ExecutablesTypes::Error>::Methods{

};

template<>
template<>
class ExecutablesFactory::InterfaceData<ExecutablesTypes::Error>::Methods{

};

#endif // ERROR_HPP
