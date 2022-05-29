#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"
#include"../actions.hpp"

// List of all views for Executables
// ---------------------------

using namespace Panels::Configuration::View::ActionsList;
using Executables = ExecutablesFactory::ListOfBases;
using View = ActionDataView<Executables>;



template<>
View::CreateFunctionTable View::createFunctionTable = {

};

template<>
View* View::create(ActionView& view, ActionRef action){
    return (createFunctionTable[FCT_toUnderlying((action)? action->type() : ActionType())])(view, action);
}
