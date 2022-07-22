#include"conditionals.hpp"

using namespace Panels::Configuration::View::ActionsList;
using Conditionals = ConditionalsFactory::ListOfBases;
using View = ActionDataView<Conditionals>;
using CompareActionDataView = CompareActionView::CompareActionView;
using CompareArgumentStatDataView = CompareArgumentStatActionView::CompareArgumentStatActionView;
using CompareAndWriteDataView = CompareAndWriteActionView;

template<>
View::CreateFunctionTable View::createFunctionTable = {
    &CompareNumbOfArgsActionView::create,
    &CompareArgumentStatDataView::create,
    &CompareActionDataView::create,
    &CompareAndWriteDataView::create
};

template<>
View* View::createView(QWidget* parent, ActionRef action){
    return (createFunctionTable[FCT_toUnderlying((action)? action->type() : ActionType())])(parent, action);
}

template<>
View* View::createView(QWidget* parent, ActionType type){
    return (createFunctionTable[FCT_toUnderlying(type)])(parent, nullptr);
}
