#include"multilineEditorDialog.hpp"
#include<QEvent>

bool MultiLineEditor::eventFilter(QObject* obj, QEvent* ev){
    switch(ev->type()){
    case QEvent::MouseButtonRelease:
    {
        if(obj == &ok){
            done(QDialog::Accepted);
        }
        if(obj == &cancel){
            done(QDialog::Rejected);
        }
    }
        break;
    default:
        break;
    }

    return QDialog::eventFilter(obj, ev);
}
