#include"attribute.hpp"

namespace FormattedString_NS = Panels::Configuration::View::FormattedString;
using AttributeItemView = FormattedString_NS::AttributeItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = AttributeItemView::FormatRuleRef;

AttributeItemView::ItemDataView* AttributeItemView::create(QWidget* parent, FormatRuleRef rule){
    return new AttributeItemView(parent, rule);
}


AttributeItemView::AttributeItemDataView(QWidget* parent, FormatRuleRef pRule)
    : AttributeItemView(parent)
{
    if(pRule){
        Rule& rule = *static_cast<Rule*>(pRule);
        nameLineEdit.setText(rule.name());
    }
}

void AttributeItemView::readRule(FormatRuleBase& fRule){
    Rule& rule = *static_cast<Rule*>(&fRule);
    rule.setName(nameLineEdit.text());
}
