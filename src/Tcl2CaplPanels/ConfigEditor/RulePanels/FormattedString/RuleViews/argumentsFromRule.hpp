#ifndef ARGUMENTSFROMRULE_HPP
#define ARGUMENTSFROMRULE_HPP

#include"../formattedString.hpp"
#include<QSpinBox>
#include<QLineEdit>

namespace Panels::Configuration::View::FormattedString{
    class ArgumentsFromItemDataView
            : public ItemDataView
    {
    public:
        ArgumentsFromItemDataView(){
            addRow("Indeks: ", &indexSpinBox);
            indexSpinBox.setRange(-99, 99);
            indexSpinBox.setValue(0);
            addRow("Separator: ", &separatorLineEdit);
            separatorLineEdit.setClearButtonEnabled(true);
            separatorLineEdit.setStyleSheet("border: 1px solid blue;");
        }
        ~ArgumentsFromItemDataView(){}

        using ItemDataView::DataRef;
        static ItemDataView* create(ItemView& view, DataRef = nullptr);
    protected:
        //QSpinBox* indexSpinBox = nullptr;
        //QLineEdit* separatorLineEdit = nullptr;
        QSpinBox indexSpinBox;// = nullptr;
        QLineEdit separatorLineEdit;// = nullptr;

    };
}

#endif // ARGUMENTSFROMRULE_HPP