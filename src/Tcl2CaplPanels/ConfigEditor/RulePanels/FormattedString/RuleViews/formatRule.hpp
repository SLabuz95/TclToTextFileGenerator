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

        using ItemDataView::DataRef;
        static ItemDataView* create(ItemView& view, DataRef = nullptr);
    protected:
        //QComboBox* formatRule = nullptr;
        QComboBox formatRule;// = nullptr;

    };
}

#endif // FORMATRULE_HPP
