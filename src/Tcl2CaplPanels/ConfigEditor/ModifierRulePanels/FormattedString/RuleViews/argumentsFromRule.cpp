#include"argumentsFromRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::ModifierFormattedString;
using ArgumentsFromRuleView = FormattedString_NS::ArgumentsFromItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = ArgumentsFromRuleView::FormatRuleRef;

ArgumentsFromRuleView::ItemDataView* ArgumentsFromRuleView::create(QWidget* parent, FormatRuleRef rule){
    return new ArgumentsFromRuleView(parent, rule);
}


ArgumentsFromRuleView::ArgumentsFromItemDataView(QWidget* parent, FormatRuleRef pRule)
    : ArgumentsFromRuleView(parent)
{
    if(pRule){
        Rule& rule = *static_cast<Rule*>(pRule);
        indexSpinBox.setValue(rule.index());
        separatorLineEdit.setText(rule.separator());
    }
}

void ArgumentsFromRuleView::readRule(FormatRuleBase& fRule){
    Rule& rule = *static_cast<Rule*>(&fRule);
    rule.setIndex(indexSpinBox.value());
    rule.setSeparator(separatorLineEdit.text());
}
