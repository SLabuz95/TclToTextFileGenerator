#include"indexRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using IndexRuleView = FormattedString_NS::IndexItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = IndexRuleView::FormatRuleRef;

IndexRuleView::ItemDataView* IndexRuleView::create(ItemView& view, FormatRuleRef){
    return new IndexRuleView();
}
