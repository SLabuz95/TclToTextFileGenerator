#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include<QMainWindow>
#include<QHBoxLayout>
#include<QStackedWidget>
#include<QBoxLayout>
#include<QPushButton>
#include"Tcl2CaplPanels/tcl2caplfileparserpanel.hpp"
#include"Tcl2CaplPanels/tcl2caplinstructionpanel.hpp"
#include<QDebug>
#include<QPushButton>

class App;

class AppWindow : public QMainWindow{

    using AppModePanel = QTabWidget;  // CentralWidget

    const QString FILE_PARSER_TAB_TEXT = QStringLiteral("Parser plików");
    const QString INSTRUCTION_PARSER_TAB_TEXT = QStringLiteral("Parser instrukcji");

    AppModePanel appModePanel;
    Tcl2CaplFileParserPanel tcl2CaplFileParserPanel;
    Tcl2CaplInstructionPanel tcl2CaplInstructionParserPanel;

    App& app;

protected:

    bool eventFilter(QObject* obj, QEvent* ev);
public:
    AppWindow(App&);

};

#endif // APPWINDOW_HPP
