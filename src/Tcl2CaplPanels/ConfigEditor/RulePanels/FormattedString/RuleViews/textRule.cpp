#include"textRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using TextRuleView = FormattedString_NS::TextItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = TextRuleView::FormatRuleRef;

TextRuleView::ItemDataView* TextRuleView::create(ItemView& view, FormatRuleRef){
    return new TextRuleView();
}


