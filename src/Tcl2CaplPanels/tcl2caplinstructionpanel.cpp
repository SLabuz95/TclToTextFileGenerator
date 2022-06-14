#include"tcl2caplinstructionpanel.hpp"
#include"tcl2caplinstructioninstance.hpp"
#include<QDockWidget>
#include<QEvent>
#include<QInputDialog>


Tcl2CaplInstructionPanel::Tcl2CaplInstructionPanel()
{
    // ToolBar Setup
    using ToolButton = ToolButton*;
    using ToolButtonText = ToolButtonTexts::const_iterator;
    ToolButton toolButton;
    ToolButtonText toolButtonText;
    for (toolButton = toolButtons, toolButtonText = toolOptionsNames.constBegin();
         toolButton < &toolButtons[numbOfToolOptions] ; toolButton++, toolButtonText++)
    {
       toolButton->setText(*toolButtonText);
       toolButton->installEventFilter(this);
       toolBar.addWidget(toolButton)->setVisible(true);
    }
    addToolBar(&toolBar);
    //toolBar.show();
    show();
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::TabPosition::North);
    //newInstance();
}

void Tcl2CaplInstructionPanel::newInstance(){
    QString tabName;
    if(!(tabName = QInputDialog::getText(nullptr, QStringLiteral("Nowy projekt"), QStringLiteral("Type tab name:"))).isNull() or !tabName.isEmpty()){
        QDockWidget* newInstance = new QDockWidget(tabName);

        Tcl2CaplInstructionInstance* instanceWidget = new Tcl2CaplInstructionInstance(*this);
         newInstance->setWidget(instanceWidget);
        //newInstance->setAttribute(Qt::WA_DeleteOnClose);
        addDockWidget(Qt::BottomDockWidgetArea, newInstance);
        show();
    }
}

bool Tcl2CaplInstructionPanel::eventFilter(QObject *obj, QEvent *ev){
    switch(ev->type()){
    case QEvent::MouseButtonPress:
    {
        if(obj >= toolButtons and obj < &toolButtons[numbOfToolOptions]){
            (this->*toolButtonsPressed[static_cast<ToolButton*>(obj) - toolButtons])();
        }
    }
        break;
    default:
        break;
    }

    return QMainWindow::eventFilter(obj, ev);
}

// PRESSED
template<>
void Tcl2CaplInstructionPanel::toolButtonPressed
<InstPanelToolOption::NewParser>()
{
    newInstance();
}

Tcl2CaplInstructionPanel::ToolButtonsFunctions Tcl2CaplInstructionPanel::toolButtonsPressed =
{
    &Tcl2CaplInstructionPanel::toolButtonPressed<InstPanelToolOption::NewParser>
};
