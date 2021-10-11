#ifndef ADDUSERINTERACTION_HPP
#define ADDUSERINTERACTION_HPP

#include"External/Factory/factory.hpp"
#include"Tcl2Capl/Config/Actions/Executable/definition.hpp"

template<>
template<>
struct ExecutablesFactory::ImplementationData<ExecutablesTypes::AddUserInteraction>::Properties{

};

template<>
template<>
class ExecutablesFactory::ImplementationData<ExecutablesTypes::AddUserInteraction>::Methods{

};

template<>
template<>
class ExecutablesFactory::InterfaceData<ExecutablesTypes::AddUserInteraction>::Methods{

};

#endif // ADDUSERINTERACTION_HPP
