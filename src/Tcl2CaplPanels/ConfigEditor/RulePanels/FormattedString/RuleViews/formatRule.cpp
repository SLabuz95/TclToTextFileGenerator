#include"formatRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using FormatRuleView = FormattedString_NS::FormatItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = FormatRuleView::ItemView;

FormatRuleView::ItemDataView* FormatRuleView::create(RuleView& view, DataRef){
    return new FormatRuleView();
}
