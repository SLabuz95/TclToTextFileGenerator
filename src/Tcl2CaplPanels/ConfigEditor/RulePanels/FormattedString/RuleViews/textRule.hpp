#ifndef TEXTRULE_HPP
#define TEXTRULE_HPP

#include"../formattedString.hpp"
#include<QLineEdit>

namespace Panels::Configuration::View::FormattedString{
    class TextItemDataView
            : public ItemDataView
    {
    public:
        TextItemDataView(){
            addRow("Tekst: ", &textLineEdit);
            textLineEdit.setClearButtonEnabled(true);
        }
        ~TextItemDataView(){}

        class LineEdit : public QLineEdit{
        public:
            LineEdit(){
                setStyleSheet("border: 1px solid red;");
            }
            bool event(QEvent* ev)override{
                if(ev->type() == QEvent::InputMethodQuery){
                    if(text().isEmpty()){
                        setStyleSheet("border: 1px solid red;");
                    }else{
                        setStyleSheet("");
                    }
                }
                return QLineEdit::event(ev);
            }
        };

        using ItemDataView::FormatRuleRef;
        static ItemDataView* create(ItemView& view, FormatRuleRef = nullptr);
        constexpr FormatRuleType type()const override{return FormatRuleType::TextItem;}
    protected:
        //LineEdit* textLineEdit = nullptr;
        LineEdit textLineEdit;// = nullptr;

    };
}

#endif // TEXTRULE_HPP
