#include"Tcl2Capl/Config/Actions/Executable/executables.hpp"
#include"../action.hpp"

// List of all views for Executables

// ---------------------------

using namespace Panels::Configuration::View::Actions;
using Executable = ExecutablesFactory::ProductBase;
using ActionView = ActionViewBase<Executable>;



template<>
ActionView::CreateFunctionTable ActionView::createFunctionTable = {

};

template<>
ActionView* ActionView::create(ActionRef action){
    return (createFunctionTable[FCT_toUnderlying((action)? action->type() : ActionType())])(action);
}
