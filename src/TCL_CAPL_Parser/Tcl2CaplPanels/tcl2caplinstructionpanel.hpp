#ifndef TCL2CAPLINSTRUCTIONPANEL_HPP
#define TCL2CAPLINSTRUCTIONPANEL_HPP

#include<QMainWindow>
#include<QPushButton>
#include<QToolBar>

class AppWindow;
class Tcl2CaplInstructionPanel : public QMainWindow{
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
    using ToolButtonFunc = void (Tcl2CaplInstructionPanel::*)();
    using ToolButtonTexts = const  QVarLengthArray<QString, numbOfToolOptions>;
    using ToolButtonsFunctions = const ToolButtonFunc[numbOfToolOptions];
protected:
    ToolButtonTexts toolOptionsNames =
    {
        QStringLiteral("Nowa konfiguracja")
    };
    using ToolButtons =  ToolButton[numbOfToolOptions];

    AppWindow& appWindow;

    QToolBar toolBar;
    ToolButtons toolButtons;

    template<ToolOptions option>
    requires (option != ToolOptions::NumbOfOptions)
    void toolButtonPressed();


    bool eventFilter(QObject* obj, QEvent* ev);

    static ToolButtonsFunctions toolButtonsPressed;


public:
    Tcl2CaplInstructionPanel(AppWindow&);

    void newInstance();
};

using InstPanelToolOption = Tcl2CaplInstructionPanel::ToolOptions;

#endif // TCL2CAPLINSTRUCTIONPANEL_HPP
