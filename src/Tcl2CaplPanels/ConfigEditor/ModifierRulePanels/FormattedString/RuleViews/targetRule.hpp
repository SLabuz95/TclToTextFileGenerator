#ifndef MODIFIER_RULE_VIEW_TARGETRULE_HPP
#define MODIFIER_RULE_VIEW_TARGETRULE_HPP

#include"../formattedString.hpp"
#include<QSpinBox>
#include<QLineEdit>

namespace Panels::Configuration::View::ModifierFormattedString{
    class TargetItemDataView
            : public ItemDataView
    {
        static constexpr FormatRuleType ruleType = FormatRuleType::TargetItem;
    public:
        TargetItemDataView(QWidget* parent)
            : ItemDataView(parent)
        {
            const QStringList targetRuleComboBoxOptions =
            {
                "Raw",
                "SplittedRaw",
            };

            addRow("Format: ", &formatRule);
            formatRule.addItems(targetRuleComboBoxOptions);
            formatRule.view()->installEventFilter(this);
        }
        TargetItemDataView(QWidget* parent, FormatRuleRef);
        ~TargetItemDataView(){}

        using ItemDataView::FormatRuleRef;
        static ItemDataView* create(QWidget* parent, FormatRuleRef = nullptr);
        using Rule = ModifierFormatParametersFactory::Product<ruleType>;
    protected:
        //QComboBox* formatRule = nullptr;
        QComboBox formatRule;// = nullptr;
    public:
        constexpr FormatRuleType type()const override{return ruleType;}
        void readRule(FormatRuleBase&) override;
    };
}

#endif // MODIFIER_RULE_VIEW_TARGETRULE_HPP
