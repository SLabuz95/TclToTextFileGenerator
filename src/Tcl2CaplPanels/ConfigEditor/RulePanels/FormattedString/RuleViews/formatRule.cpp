#include"formatRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using FormatRuleDataView = FormattedString_NS::FormatItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = FormatRuleDataView::FormatRuleRef;

FormatRuleDataView::ItemDataView* FormatRuleDataView::create(ItemView& view, FormatRuleRef){
    return new FormatRuleDataView();
}
