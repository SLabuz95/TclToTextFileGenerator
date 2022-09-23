#include"targetRule.hpp"

namespace FormattedString_NS = Panels::Configuration::View::ModifierFormattedString;
using TargetItemDataView = FormattedString_NS::TargetItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = TargetItemDataView::FormatRuleRef;

TargetItemDataView::ItemDataView* TargetItemDataView::create(QWidget* parent, FormatRuleRef rule){
    return new TargetItemDataView(parent, rule);
}

TargetItemDataView::TargetItemDataView(QWidget* parent, FormatRuleRef pRule)
    : TargetItemDataView(parent)
{
    if(pRule){
        Rule& rule = *static_cast<Rule*>(pRule);
        formatRule.setCurrentIndex(Rule::Format::cast_target(rule.target()));
    }
}

void TargetItemDataView::readRule(FormatRuleBase& fRule){
    Rule& rule = *static_cast<Rule*>(&fRule);
    rule.setTarget(Rule::Format::castTo_target(formatRule.currentIndex()));
}
