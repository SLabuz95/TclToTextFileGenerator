#include"argumentsFromRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using ArgumentsFromRuleView = FormattedString_NS::ArgumentsFromItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = ArgumentsFromRuleView::ItemView;

ArgumentsFromRuleView::ItemDataView* ArgumentsFromRuleView::create(RuleView& view, DataRef){
    return new ArgumentsFromRuleView();
}
