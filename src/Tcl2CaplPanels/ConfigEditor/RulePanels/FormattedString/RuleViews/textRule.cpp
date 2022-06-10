#include"textRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using TextRuleView = FormattedString_NS::TextItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = TextRuleView::ItemView;

TextRuleView::ItemDataView* TextRuleView::create(RuleView& view, DataRef){
    return new TextRuleView();
}


