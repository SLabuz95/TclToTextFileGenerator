#include"conditionals.hpp"

using namespace Panels::Configuration::View::ActionsList;
using Conditionals = ConditionalsFactory::ListOfBases;
using View = ActionDataView<Conditionals>;
using CompareActionDataView = CompareActionView::CompareActionView;
using IsLastSavedStatDataView = IsLastSavedStatActionView::IsLastSavedStatActionView;

template<>
View::CreateFunctionTable View::createFunctionTable = {
    &CompareNumbOfArgsActionView::create,
    &IsLastSavedStatDataView::create,
    &CompareActionDataView::create
};

template<>
View* View::createView(ActionView& view, ActionRef action){
    return (createFunctionTable[FCT_toUnderlying((action)? action->type() : ActionType())])(view, action);
}
