#include"formatRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using FormatRuleDataView = FormattedString_NS::FormatItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = FormatRuleDataView::FormatRuleRef;

FormatRuleDataView::ItemDataView* FormatRuleDataView::create(QWidget* parent, FormatRuleRef rule){
    return new FormatRuleDataView(parent, rule);
}

FormatRuleDataView::FormatItemDataView(QWidget* parent, FormatRuleRef pRule)
    : FormatItemDataView(parent)
{
    if(pRule){
        Rule& rule = *static_cast<Rule*>(pRule);
        formatRule.setCurrentIndex(Rule::Format::cast_target(rule.target()));
    }
}

void FormatRuleDataView::readRule(FormatRuleBase& fRule){
    Rule& rule = *static_cast<Rule*>(&fRule);
    rule.setTarget(Rule::Format::castTo_target(formatRule.currentIndex()));
}
