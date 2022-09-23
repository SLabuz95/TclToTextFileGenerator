#include"indexRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using IndexRuleView = FormattedString_NS::IndexItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = IndexRuleView::FormatRuleRef;

IndexRuleView::ItemDataView* IndexRuleView::create(QWidget* parent, FormatRuleRef rule){
    return new IndexRuleView(parent, rule);
}

IndexRuleView::IndexItemDataView(QWidget* parent,FormatRuleRef pRule)
    : IndexItemDataView(parent)
{
    if(pRule){
        Rule& rule = *static_cast<Rule*>(pRule);
        indexSpinBox.setValue(rule.index());
    }
}


void IndexRuleView::readRule(FormatRuleBase& fRule){
    Rule& rule = *static_cast<Rule*>(&fRule);
    rule.setIndex(indexSpinBox.value());
}
