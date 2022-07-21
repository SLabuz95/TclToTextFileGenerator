#include"conditionals.hpp"

using namespace Panels::Configuration::View::ActionsList;
using Conditionals = ConditionalsFactory::ListOfBases;
using View = ActionDataView<Conditionals>;
using CompareActionDataView = CompareActionView::CompareActionView;
using IsLastSavedStatDataView = IsLastSavedStatActionView::IsLastSavedStatActionView;
using CompareAndWriteDataView = CompareAndWriteActionView;

template<>
View::CreateFunctionTable View::createFunctionTable = {
    &CompareNumbOfArgsActionView::create,
    /*&IsLastSavedStatDataView::create, replaced by*/&createNoDataView,
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
