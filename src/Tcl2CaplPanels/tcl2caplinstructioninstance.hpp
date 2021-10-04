#ifndef TCL2CAPLINSTRUCTIONINSTANCE_HPP
#define TCL2CAPLINSTRUCTIONINSTANCE_HPP


#include<QMainWindow>
#include<QGroupBox>
#include<QBoxLayout>
#include<QGridLayout>
#include<QLineEdit>
#include<QPushButton>
#include<QLabel>
#include<QTextEdit>
#include<QSplitter>
//#include"Tcl2Capl/userdefinitions_helper.hpp"
#include<QMimeData>
#include<QCompleter>
#include<QResizeEvent>

class Tcl2CaplInstructionPanel;


class Tcl2CaplInstructionInstance : public QMainWindow{
    class InputTextEdit : public QTextEdit{
        //TODO: Dodac notVisible, zsynhronizowac z rodzicem i completerem i sprawdzic czy completer dziala
    public:
        InputTextEdit() : QTextEdit(), completerHelper(this){
            completerHelper.setGeometry(this->rect().bottomLeft().x(), this->rect().bottomLeft().y() - completerHelper.height(),
                                        this->rect().width(), completerHelper.height());
            completerHelper.setFocusPolicy(Qt::NoFocus);
        }

    protected:
        QLineEdit completerHelper;
        QCompleter completer;

        void insertFromMimeData(const QMimeData* data)override{
            // Replace "\x93" and "\x94"  coding from text to "\"" (copied from word document etc)
            QTextEdit::insertFromMimeData(data);
            QString str = toPlainText().toUtf8();
            str.replace(QString(QByteArray("\xE2\x80\x9C")), "\"").replace(QString(QByteArray("\xE2\x80\x9D")), "\"");
            setPlainText(str);
         }

        void resizeEvent(QResizeEvent* ev)override{
            completerHelper.setGeometry(this->rect().bottomLeft().x(), this->rect().bottomLeft().y() - completerHelper.height(),
                                       this->rect().width(), completerHelper.height());
            QTextEdit::resizeEvent(ev);
        }

    };

    using Super = QMainWindow;
    using CentralWidget = QWidget;

    using InputTclConfigBox = QGroupBox;
    using InputTclConfig = InputTextEdit;
    using InputConfigBox = QGroupBox;
    using InputConfig = InputTextEdit;

    using GenerateButton = QPushButton;
    using GenerateRaportModeButton = QPushButton;

    using OutputBox = QGroupBox;
    using OutputText = QTextEdit;
    using ErrorsBox = QGroupBox;
    using ErrorsText = QTextEdit;

    const QString INPUT_TCL_CONFIG_BOX_TEXT = QStringLiteral("Skrypt TCL");
    const QString INPUT_CONFIG_BOX_TITLE = QStringLiteral("Skrypt konfiguracyjny");
    const QString GENERATE_CAPL_BUTTON_TEXT = QStringLiteral("Generuj skrypty CAPL");
    const QString GENERATE_CAPL_WRITE_ONLY_MODE_BUTTON_TEXT = QStringLiteral("Generuj skrypty CAPL - Tryb raportowy");
    const QString OUTPUT_BOX_TITLE = QStringLiteral("Skrypt CAPL");
    const QString ERRORS_BOX_TITLE = QStringLiteral("Błędy");
    bool active  = false;
    Tcl2CaplInstructionPanel& panel;

    CentralWidget centralWidget;
    QVBoxLayout centralWidgetLayout;
    QSplitter mainSplitter;
    QHBoxLayout controlLayout;
    QSplitter tclCaplSplitter;
    QSplitter configErrorsSplitter;

    InputTclConfigBox inputTclConfigBox;
    QVBoxLayout inputTclBoxLayout;
    OutputBox outputBox;
    QVBoxLayout outputBoxLayout;
    InputConfigBox inputConfigBox;
    QVBoxLayout inputConfigBoxLayout;
    ErrorsBox errorsBox;
    QVBoxLayout errorsBoxLayout;

    InputTclConfig inputTclConfig;
    OutputText outputText;
    GenerateButton generateCaplButton;
    GenerateRaportModeButton generateCaplRaportModeButton;
    InputConfig inputConfig;
    ErrorsText errorsText;

    QCompleter inputConfigCompleter;
    QLineEdit completerHelper;

    // Temp Data from ConfigFile
    //using UserProceduresConfig = UserDefinitionsData;
    //UserProceduresConfig tempUserProceduresConfig_;

public:
    using Error = QString;
    Tcl2CaplInstructionInstance(Tcl2CaplInstructionPanel& panel);

    void generateCapl();
    void generateCaplRaportMode();
    void setDefaultOutput();

protected:
    bool eventFilter(QObject*, QEvent*);
    bool readInputConfig();
};

#endif // TCL2CAPLINSTRUCTIONINSTANCE_HPP
