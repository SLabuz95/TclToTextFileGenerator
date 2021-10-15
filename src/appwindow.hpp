#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include<QMainWindow>
#include<QHBoxLayout>
#include<QStackedWidget>
#include<QBoxLayout>
#include<QPushButton>
#include<QDebug>
#include<QPushButton>
#include<QToolBar>
#include<QLabel>
#include"Tcl2CaplPanels/FileWatcher/fileWatcher.hpp"

class App;
class Tcl2CaplInstructionPanel;
class AppWindow : public QMainWindow{
public:
    enum class ToolOptions : uint{
        Begin,
        NewParser = ToolOptions::Begin,
        RuntimeInterpreter,

        NumbOfOptions
    };
protected:
    constexpr static std::underlying_type_t<ToolOptions> numbOfToolOptions = static_cast<std::underlying_type_t<ToolOptions>>(ToolOptions::NumbOfOptions);
    constexpr static std::underlying_type_t<ToolOptions> toUnderlyingType(ToolOptions option){return static_cast<std::underlying_type_t<ToolOptions>>(option);}
public:
    using ToolButton = QPushButton;
    using ToolButtonFunc = void (AppWindow::*)();
    using ToolButtonTexts = const  QVarLengthArray<QString, numbOfToolOptions>;
    using ToolButtonsFunctions = const ToolButtonFunc[numbOfToolOptions];
protected:
    ToolButtonTexts toolOptionsNames =
    {
        QStringLiteral("Nowa konfiguracja"),
        QStringLiteral("Interpreter")
    };
    using ToolButtons =  ToolButton[numbOfToolOptions];

    QToolBar toolBar;
    ToolButtons toolButtons;

    template<ToolOptions option>
    requires (option != ToolOptions::NumbOfOptions)
    void toolButtonPressed();

   /* Old Implementation
    using AppModePanel = QTabWidget;  // CentralWidget

    const QString FILE_PARSER_TAB_TEXT = QStringLiteral("Parser plików");
    //const QString INSTRUCTION_PARSER_TAB_TEXT = QStringLiteral("Parser instrukcji");
*/
    //AppModePanel appModePanel;
    //Tcl2CaplFileParserPanel tcl2CaplFileParserPanel;
    //Tcl2CaplInstructionPanel tcl2CaplInstructionParserPanel;

    App& app;
    Tcl2CaplInstructionPanel* instructionPanel = nullptr;

    static ToolButtonsFunctions toolButtonsPressed;


protected:

    bool eventFilter(QObject* obj, QEvent* ev);
public:
    AppWindow(App&);
    ~AppWindow(){
        //fileWatcher.quit();
        //fileWatcher.wait();
    }
    void newInstance();

    // FileWatcherRequests
/*
    void execRequest_FileWatcher_addInstance(Tcl2CaplFileParserInstance* instance, QFileInfo root);
    void execRequest_FileWatcher_removeInstance(Tcl2CaplFileParserInstance*);
    void execRequest_FileWatcher_addFile(Tcl2CaplFileParserInstance*, QFileInfo, bool);
    void execRequest_FileWatcher_editFile(Tcl2CaplFileParserInstance*, QFileInfo, QFileInfo, bool);
    void execRequest_FileWatcher_removeFile(Tcl2CaplFileParserInstance*, QFileInfo, bool);
    void execRequest_FileWatcher_update(Tcl2CaplFileParserInstance*);*/
};

using PanelToolOption = AppWindow::ToolOptions;

/*template<PanelToolOption option>
requires (option != AppWindow::ToolOptions::NumbOfOptions)
void AppWindow::toolButtonPressed<option>();*/

#endif // APPWINDOW_HPP
