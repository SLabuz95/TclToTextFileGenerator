#ifndef TEXTRULE_HPP
#define TEXTRULE_HPP

#include"../formattedString.hpp"

template<>
class OutputContent
<CompareAndWriteActionView::OutputsList::ListItem::ItemType::PlainTextItem>
        : public ItemDataView
{
public:
    OutputContent() : textLineEdit(new LineEdit){
        addRow("Tekst: ", textLineEdit);
        textLineEdit->setClearButtonEnabled(true);
    }
    ~OutputContent(){}

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

protected:
    LineEdit* textLineEdit = nullptr;

};

#endif // TEXTRULE_HPP
