#include"conditionals.hpp"

using namespace Panels::Configuration::View::ActionsList;
using Conditionals = ConditionalsFactory::ListOfBases;
using View = ActionDataView<Conditionals>;


template<>
View::CreateFunctionTable View::createFunctionTable = {
    &CompareNumbOfArgsActionView::create
};

template<>
View* View::create(ActionView& view,ActionRef action){
    return (createFunctionTable[FCT_toUnderlying((action)? action->type() : ActionType())])(view, action);
}
