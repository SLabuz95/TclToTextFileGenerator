#include"tcl2caplfileparserpanel.hpp"
#include"tcl2caplfileparserinstance.hpp"
#include<QDockWidget>
#include<QEvent>
#include<QInputDialog>
/*
Tcl2CaplFileParserPanel::Tcl2CaplFileParserPanel(AppWindow& appWindow)
    : appWindow(appWindow)
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
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::TabPosition::North);
    //newInstance();
    //fileWatcher.start();
}

void Tcl2CaplFileParserPanel::newInstance(){
    QString tabName;
    if(!(tabName = QInputDialog::getText(nullptr, QStringLiteral("Nowy projekt"), QStringLiteral("Type tab name:"))).isNull() or !tabName.isEmpty()){
        QDockWidget* newInstance = new QDockWidget(tabName);
        Tcl2CaplFileParserInstance* instanceWidget = new Tcl2CaplFileParserInstance(*this);
         newInstance->setWidget(instanceWidget);
        //newInstance->setAttribute(Qt::WA_DeleteOnClose);
        addDockWidget(Qt::TopDockWidgetArea, newInstance);

        show();
    }


}

bool Tcl2CaplFileParserPanel::eventFilter(QObject *obj, QEvent *ev){
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
/*
void Tcl2CaplFileParserPanel::execRequest_FileWatcher_addInstance(Tcl2CaplFileParserInstance* instance, QFileInfo root){
    QApplication::postEvent(&fileWatcher, new FileWatcher::NewInstanceEvent(instance, root));
}
void Tcl2CaplFileParserPanel::execRequest_FileWatcher_removeInstance(Tcl2CaplFileParserInstance* instance){
    QApplication::postEvent(&fileWatcher, new FileWatcher::RemoveInstanceEvent(instance));
}
void Tcl2CaplFileParserPanel::execRequest_FileWatcher_addFile(Tcl2CaplFileParserInstance*, QFileInfo, bool){

}
void Tcl2CaplFileParserPanel::execRequest_FileWatcher_editFile(Tcl2CaplFileParserInstance*, QFileInfo, QFileInfo, bool){

}
void Tcl2CaplFileParserPanel::execRequest_FileWatcher_removeFile(Tcl2CaplFileParserInstance*, QFileInfo, bool){

}
void Tcl2CaplFileParserPanel::execRequest_FileWatcher_update(Tcl2CaplFileParserInstance*){

}*/

// PRESSED
/*
template<>
void Tcl2CaplFileParserPanel::toolButtonPressed
<PanelToolOption::NewParser>()
{
    newInstance();
}



Tcl2CaplFileParserPanel::ToolButtonsFunctions Tcl2CaplFileParserPanel::toolButtonsPressed =
{
    &Tcl2CaplFileParserPanel::toolButtonPressed<PanelToolOption::NewParser>,
};
*/
