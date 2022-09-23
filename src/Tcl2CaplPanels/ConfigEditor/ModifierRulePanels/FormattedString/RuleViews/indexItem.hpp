#ifndef MODIFIER_RULE_VIEW_INDEXITEM_HPP
#define MODIFIER_RULE_VIEW_INDEXITEM_HPP


#include"../formattedString.hpp"
#include"External/Improvements/spinBoxNoWheel.hpp"

namespace Panels::Configuration::View::ModifierFormattedString{
    class IndexItemDataView
            : public ItemDataView
    {
        static constexpr FormatRuleType ruleType = FormatRuleType::IndexItem;
    public:
        using ItemDataView::FormatRuleRef;
        IndexItemDataView(QWidget* parent)
            : ItemDataView(parent)
        {
            addRow("Indeks: ", &indexSpinBox);
            indexSpinBox.setRange(-99, 99);
            indexSpinBox.setValue(0);
        }
        IndexItemDataView(QWidget* parent, FormatRuleRef);
        ~IndexItemDataView(){}

        static ItemDataView* create(QWidget* parent, FormatRuleRef);
        constexpr FormatRuleType type()const override{return ruleType;}
        using Rule = ModifierFormatParametersFactory::Product<ruleType>;
        void readRule(FormatRuleBase&) override;
    protected:
        //QSpinBox* indexSpinBox = nullptr;
        SpinBoxNoWheel indexSpinBox;// = nullptr;
    };
}

#endif // MODIFIER_RULE_VIEW_INDEXITEM_HPP
