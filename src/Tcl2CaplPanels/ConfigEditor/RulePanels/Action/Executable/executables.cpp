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
    &AddPredefinitionActionView::create,
    &WriteAttributeActionView::create,
};

template<>
View* View::createView(QWidget* parent, ActionRef action){
    return (createFunctionTable[FCT_toUnderlying((action)? action->type() : ActionType())])(parent, action);
}

template<>
View* View::createView(QWidget* parent, ActionType type){
    return (createFunctionTable[FCT_toUnderlying(type)])(parent, nullptr);
}
