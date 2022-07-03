#include"argumentsFromRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using ArgumentsFromRuleView = FormattedString_NS::ArgumentsFromItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = ArgumentsFromRuleView::FormatRuleRef;

ArgumentsFromRuleView::ItemDataView* ArgumentsFromRuleView::create(ItemView& view, FormatRuleRef){
    return new ArgumentsFromRuleView();
}
