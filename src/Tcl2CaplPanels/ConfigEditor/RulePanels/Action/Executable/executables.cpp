
#include"executables.hpp"

using namespace Panels::Configuration::View::ActionsList;
using Executables = ExecutablesFactory::ListOfBases;
using View = ActionDataView<Executables>;


template<>
View::CreateFunctionTable View::createFunctionTable = {
    &WriteActionView::create,
    &TclParseActionView::create,
    &ErrorActionView::create,
    &AddPreexpressionActionView::create,
    &AddUserInteractionActionView::create,
    &CompareAndWriteActionView::create,
};

template<>
View* View::createView(ActionView& view, ActionRef action){
    return (createFunctionTable[FCT_toUnderlying((action)? action->type() : ActionType())])(view, action);
}