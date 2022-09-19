#include"Conditional/conditionals.hpp"
#include"Executable/executables.hpp"
#include"Supp/supps.hpp"

using namespace Panels::Configuration::View::ModifierActionsList;
using ActionsModifier = ModifierActionsFactory::ListOfBases;
using View = ActionDataView<ActionsModifier>;
using CompareActionDataView = CompareActionView::CompareActionView;
using StartsWithActionDataView = StartsWithActionView::StartsWithActionView;

template<>
View::CreateFunctionTable View::createFunctionTable =
{
    &SplitActionView::create,
    &WriteAttributeActionView::create,
    &StartsWithActionDataView::create,
    &CompareActionDataView::create,
    &WriteActionView::create,
    &ChangePhaseActionView::create,
    &InterpretActionView::create,
};

template<>
View* View::createView(QWidget* parent, ActionRef action){
    return (createFunctionTable[FCT_toUnderlying((action)? action->type() : ActionType())])(parent, action);
}

template<>
View* View::createView(QWidget* parent, ActionType type){
    return (createFunctionTable[FCT_toUnderlying(type)])(parent, nullptr);
}
