#include"rules.hpp"

using namespace Panels::Configuration::View::FormattedString;
using FormatRules = FormatParametersFactory::ListOfBases;
using View = ItemDataView;


View::CreateFunctionTable View::createFunctionTable = {
    &TextItemDataView::create,
    &createNoDataView,
    &IndexItemDataView::create,
    &ArgumentsFromItemDataView::create,
    &FormatItemDataView::create,
};

View* View::createView(ItemView& view,DataRef data){
    return (createFunctionTable[FCT_toUnderlying((data)? data->type() : DataViewType())])(view, data);
}
