#include"indexRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using IndexRuleView = FormattedString_NS::IndexItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = IndexRuleView::ItemView;

IndexRuleView::ItemDataView* IndexRuleView::create(RuleView& view, DataRef){
    return new IndexRuleView();
}
