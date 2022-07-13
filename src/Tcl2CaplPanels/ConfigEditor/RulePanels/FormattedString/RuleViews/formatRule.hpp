#ifndef FORMATRULE_HPP
#define FORMATRULE_HPP

#include"../formattedString.hpp"
#include<QSpinBox>
#include<QLineEdit>

namespace Panels::Configuration::View::FormattedString{
    class FormatItemDataView
            : public ItemDataView
    {
        static constexpr FormatRuleType ruleType = FormatRuleType::FormatItem;
    public:
        FormatItemDataView(QWidget* parent)
            : ItemDataView(parent)
        {
            const QStringList formatRuleComboBoxOptions =
            {
                "Raw",    // Orginal Interpreter Read
                "TclFormat",
                "CaplFormat",
                "ProcedureParametersStat",    // Number
                //"Command",
                "SnprintfFormat"
            };

            addRow("Format: ", &formatRule);
            formatRule.addItems(formatRuleComboBoxOptions);
            formatRule.view()->installEventFilter(this);
        }
        FormatItemDataView(QWidget* parent, FormatRuleRef);
        ~FormatItemDataView(){}

        using ItemDataView::FormatRuleRef;
        static ItemDataView* create(QWidget* parent, FormatRuleRef = nullptr);
        using Rule = FormatParametersFactory::Product<ruleType>;
    protected:
        //QComboBox* formatRule = nullptr;
        QComboBox formatRule;// = nullptr;
    public:
        constexpr FormatRuleType type()const override{return ruleType;}
        void readRule(FormatRuleBase&) override;
    };
}

#endif // FORMATRULE_HPP
