#ifndef MODIFIER_RULE_VIEW_ATTRIBUTE_HPP
#define MODIFIER_RULE_VIEW_ATTRIBUTE_HPP
#include"../formattedString.hpp"
#include"External/Improvements/spinBoxNoWheel.hpp"
#include<QLineEdit>

namespace Panels::Configuration::View::ModifierFormattedString{
    class AttributeItemDataView
            : public ItemDataView
    {
        static constexpr FormatRuleType ruleType = FormatRuleType::AttributeItem;
    public:
        AttributeItemDataView(QWidget* parent)
            : ItemDataView(parent)
        {
            addRow("Nazwa: ", &nameLineEdit);
            nameLineEdit.setPlaceholderText("Atrybut");
            nameLineEdit.setClearButtonEnabled(true);
        }
        AttributeItemDataView(QWidget* parent,FormatRuleRef);
        ~AttributeItemDataView(){}

        using ItemDataView::FormatRuleRef;
        static ItemDataView* create(QWidget* parent, FormatRuleRef = nullptr);
        using Rule = ModifierFormatParametersFactory::Product<ruleType>;
    protected:
        //QSpinBox* indexSpinBox = nullptr;
        //QLineEdit* separatorLineEdit = nullptr;
        QLineEdit nameLineEdit;// = nullptr;
    public:
        constexpr FormatRuleType type()const override{return ruleType;}
        void readRule(FormatRuleBase&) override;
    };
}

#endif // MODIFIER_RULE_VIEW_ATTRIBUTE_HPP
