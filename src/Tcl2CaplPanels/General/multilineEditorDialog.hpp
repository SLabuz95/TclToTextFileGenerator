#ifndef MULTILINEEDITORDIALOG_HPP
#define MULTILINEEDITORDIALOG_HPP

#include<QDialog>
#include<QVBoxLayout>
#include<QLabel>
#include<QTextEdit>
#include<QPushButton>

class MultiLineEditor : public QDialog{
public:
    MultiLineEditor(QString text)
        : QDialog()
    {
        QTextOption to;
        to.setFlags(QTextOption::IncludeTrailingSpaces | QTextOption::ShowTabsAndSpaces);
        setSizeGripEnabled(true);
        setModal(true);

        titleLabel.setText("Edytor");

        ok.setText("Ok");
        cancel.setText("Cancel");
        buttons.addWidget(&ok);
        buttons.addWidget(&cancel);

        layout.addWidget(&titleLabel);
        layout.addWidget(&textEditor);
        layout.addLayout(&buttons);
        ok.installEventFilter(this);
        cancel.installEventFilter(this);

        setLayout(&layout);
        textEditor.document()->setDefaultTextOption(to);
        textEditor.setPlainText(text);
    }

protected:
    QVBoxLayout layout;
    QLabel titleLabel;
    QTextEdit textEditor;
    QHBoxLayout buttons;
    QPushButton ok;
    QPushButton cancel;

    bool eventFilter(QObject* obj, QEvent* ev)override;
public:
    QString text()const{return textEditor.toPlainText();}
};
#endif // MULTILINEEDITORDIALOG_HPP
