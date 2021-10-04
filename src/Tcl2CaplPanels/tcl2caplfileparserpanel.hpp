#ifndef TCL2CAPLFILEPARSERPANEL_HPP
#define TCL2CAPLFILEPARSERPANEL_HPP

#include<QMainWindow>
#include<QToolBar>
#include<QLabel>
#include<QPushButton>
#include"FileWatcher/fileWatcher.hpp"

class AppWindow;
class Tcl2CaplFileParserPanel : public QMainWindow{
public:
    enum class ToolOptions : uint{
        Begin,
        NewParser = ToolOptions::Begin,

        NumbOfOptions
    };
protected:
    constexpr static std::underlying_type_t<ToolOptions> numbOfToolOptions = static_cast<std::underlying_type_t<ToolOptions>>(ToolOptions::NumbOfOptions);
    constexpr static std::underlying_type_t<ToolOptions> toUnderlyingType(ToolOptions option){return static_cast<std::underlying_type_t<ToolOptions>>(option);}
public:
    using ToolButton = QPushButton;
    using ToolButtonFunc = void (Tcl2CaplFileParserPanel::*)();
    using ToolButtonTexts = const  QVarLengthArray<QString, numbOfToolOptions>;
    using ToolButtonsFunctions = const ToolButtonFunc[numbOfToolOptions];
protected:
    ToolButtonTexts toolOptionsNames =
    {
        QStringLiteral("Nowa konfiguracja"),
    };
    using ToolButtons =  ToolButton[numbOfToolOptions];

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
};

using PanelToolOption = Tcl2CaplFileParserPanel::ToolOptions;

/*template<PanelToolOption option>
void Tcl2CaplFileParserPanel::toolButtonPressed<option>();*/

#endif // TCL2CAPLFILEPARSERPANEL_HPP
