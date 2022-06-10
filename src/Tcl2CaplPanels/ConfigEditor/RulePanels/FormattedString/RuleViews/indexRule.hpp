#ifndef INDEXRULE_HPP
#define INDEXRULE_HPP

#include"../formattedString.hpp"
#include<QSpinBox>

namespace Panels::Configuration::View::FormattedString{
    class IndexItemDataView
            : public ItemDataView
    {
    public:
        IndexItemDataView(){
            addRow("Indeks: ", &indexSpinBox);
            indexSpinBox.setRange(-99, 99);
            indexSpinBox.setValue(0);
        }
        ~IndexItemDataView(){}

        using ItemDataView::DataRef;
        static ItemDataView* create(ItemView& view, DataRef = nullptr);
    protected:
        //QSpinBox* indexSpinBox = nullptr;
        QSpinBox indexSpinBox;// = nullptr;
    };
}

#endif // INDEXRULE_HPP
