#ifndef FORMATRULE_HPP
#define FORMATRULE_HPP

#include"../formattedString.hpp"
#include<QSpinBox>
#include<QLineEdit>

namespace Panels::Configuration::View::FormattedString{
    class FormatItemDataView
            : public ItemDataView
    {
    public:
        FormatItemDataView(){
            const QStringList formatRuleComboBoxOptions =
            {
                "Raw",    // Orginal Interpreter Read
                "TclFormat",
                "CaplFormat",
                "ProcedureParametersStat",    // Number
                "Command",
                "SnprintfFormat"
            };

            addRow("Format: ", &formatRule);
            formatRule.addItems(formatRuleComboBoxOptions);
            formatRule.view()->installEventFilter(this);
        }
        ~FormatItemDataView(){}

        using ItemDataView::FormatRuleRef;
        static ItemDataView* create(ItemView& view, FormatRuleRef = nullptr);
    protected:
        //QComboBox* formatRule = nullptr;
        QComboBox formatRule;// = nullptr;
    public:
        constexpr FormatRuleType type()const override{return FormatRuleType::FormatItem;}
    };
}

#endif // FORMATRULE_HPP
