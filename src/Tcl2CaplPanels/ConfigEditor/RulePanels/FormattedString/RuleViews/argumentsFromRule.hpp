#ifndef ARGUMENTSFROMRULE_HPP
#define ARGUMENTSFROMRULE_HPP

#include"../formattedString.hpp"
#include"External/Improvements/spinBoxNoWheel.hpp"
#include<QLineEdit>

namespace Panels::Configuration::View::FormattedString{
    class ArgumentsFromItemDataView
            : public ItemDataView
    {        
        static constexpr FormatRuleType ruleType = FormatRuleType::ArgumentsFromItem;
    public:
        ArgumentsFromItemDataView(QWidget* parent)
            : ItemDataView(parent)
        {
            addRow("Indeks: ", &indexSpinBox);
            indexSpinBox.setRange(-99, 99);
            indexSpinBox.setValue(0);
            addRow("Separator: ", &separatorLineEdit);
            separatorLineEdit.setClearButtonEnabled(true);
            separatorLineEdit.setStyleSheet("border: 1px solid blue;");
        }
        ArgumentsFromItemDataView(QWidget* parent,FormatRuleRef);
        ~ArgumentsFromItemDataView(){}

        using ItemDataView::FormatRuleRef;
        static ItemDataView* create(QWidget* parent, FormatRuleRef = nullptr);
        using Rule = FormatParametersFactory::Product<ruleType>;
    protected:
        //QSpinBox* indexSpinBox = nullptr;
        //QLineEdit* separatorLineEdit = nullptr;
        SpinBoxNoWheel indexSpinBox;// = nullptr;
        QLineEdit separatorLineEdit;// = nullptr;
    public:
        constexpr FormatRuleType type()const override{return ruleType;}
        void readRule(FormatRuleBase&) override;
    };
}

#endif // ARGUMENTSFROMRULE_HPP
