#include"textItem.hpp"

namespace FormattedString_NS = Panels::Configuration::View::ModifierFormattedString;
using TextRuleView = FormattedString_NS::TextItemDataView;
//using ListOfIndexes = CompareNumbOfArgsActionView::ListOfIndexes;
using RuleView = TextRuleView::FormatRuleRef;

TextRuleView::ItemDataView* TextRuleView::create(QWidget* parent,FormatRuleRef rule){
    return new TextRuleView(parent, rule);
}

TextRuleView::TextItemDataView(QWidget* parent,FormatRuleRef pRule)
    : TextItemDataView(parent)
{
    if(pRule){
        Rule& rule = *static_cast<Rule*>(pRule);
        textLineEdit.setText(rule.text());
    }
}

void TextRuleView::readRule(FormatRuleBase& fRule){
    Rule& rule = *static_cast<Rule*>(&fRule);
    rule.setText(textLineEdit.toPlainText());
}
