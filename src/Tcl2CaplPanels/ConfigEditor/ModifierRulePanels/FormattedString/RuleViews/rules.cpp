#include"rules.hpp"

using namespace Panels::Configuration::View::ModifierFormattedString;
using FormatRules = ModifierFormatParametersFactory::ListOfBases;
using View = ItemDataView;


View::CreateFunctionTable View::createFunctionTable = {
    &TextItemDataView::create,
    &createNoDataView,
    &IndexItemDataView::create,
    &ArgumentsFromItemDataView::create,
    &TargetItemDataView::create,    
    &AttributeItemDataView::create,
};

View* View::createView(QWidget* parent, FormatRuleRef data){
    return (createFunctionTable[FCT_toUnderlying((data)? data->type() : FormatRuleType())])(parent, data);
}

View* View::createView(QWidget* parent, FormatRuleType type){
    return (createFunctionTable[FCT_toUnderlying(type)])(parent, nullptr);
}
