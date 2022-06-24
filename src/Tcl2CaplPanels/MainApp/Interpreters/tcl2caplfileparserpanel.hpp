#ifndef TCL2CAPLFILEPARSERPANEL_HPP
#define TCL2CAPLFILEPARSERPANEL_HPP
/* Old Implementation
 *
#include<QMainWindow>
#include<QToolBar>
#include<QLabel>
#include<QPushButton>
#include"FileWatcher/fileWatcher.hpp"

class AppWindow;
class Tcl2CaplFileParserPanel : public QMainWindow{

    AppWindow& appWindow;
   // FileWatcher fileWatcher;

    QToolBar toolBar;
    ToolButtons toolButtons;

    template<ToolOptions option>
    requires (option != ToolOptions::NumbOfOptions)
    void toolButtonPressed();

    bool eventFilter(QObject* obj, QEvent* ev);

    static ToolButtonsFunctions toolButtonsPressed;

public:
    Tcl2CaplFileParserPanel(AppWindow&);
    ~Tcl2CaplFileParserPanel(){
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
/*};

using PanelToolOption = Tcl2CaplFileParserPanel::ToolOptions;
*/
/*template<PanelToolOption option>
void Tcl2CaplFileParserPanel::toolButtonPressed<option>();*/

#endif // TCL2CAPLFILEPARSERPANEL_HPP
