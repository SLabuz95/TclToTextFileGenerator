#include"appwindow.hpp"
#include"app.hpp"
#include<QDockWidget>
#include<QTabBar>
#include<QEvent>
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"
#include"Tcl2CaplPanels/tcl2caplfileparserinstance.hpp"

#include<QEvent>
#include<QInputDialog>

AppWindow::AppWindow(App& app)
    : app(app)//,
      //appModePanel(this),
      //tcl2CaplFileParserPanel(*this)//,
      //tcl2CaplInstructionParserPanel(*this)
{
    // Setup:
    // Check Initialization List    

    // - AppModePanel
    /*appModePanel.setDocumentMode(true);
    appModePanel.tabBar()->setExpanding(true);
    appModePanel.addTab(&tcl2CaplFileParserPanel, FILE_PARSER_TAB_TEXT);
    appModePanel.addTab(&tcl2CaplInstructionParserPanel, INSTRUCTION_PARSER_TAB_TEXT);*/

    //setCentralWidget(&appModePanel);
    //setCentralWidget(&tcl2CaplFileParserPanel);

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
    installEventFilter(this);

    show();
}

void AppWindow::newInstance(){
    QString tabName;
    if(!(tabName = QInputDialog::getText(nullptr, QStringLiteral("Nowy projekt"), QStringLiteral("Type tab name:"))).isNull() or !tabName.isEmpty()){
        QDockWidget* newInstance = new QDockWidget(tabName);
        Tcl2CaplFileParserInstance* instanceWidget = new Tcl2CaplFileParserInstance(*this);
        newInstance->setAttribute(Qt::WA_DeleteOnClose);
        newInstance->setWidget(instanceWidget);
        addDockWidget(Qt::TopDockWidgetArea, newInstance);

        show();
    }


}

bool AppWindow::eventFilter(QObject *obj, QEvent *ev){
    switch(ev->type()){
    case QEvent::MouseButtonPress:
    {
        if(obj >= toolButtons and obj < &toolButtons[numbOfToolOptions]){
            (this->*toolButtonsPressed[static_cast<ToolButton*>(obj) - toolButtons])();
        }
    }
        break;
    default:
        if(obj == this and ev->type() == QEvent::Close){
            app.closeAllWindows();
        }
        break;
    }
    return QMainWindow::eventFilter(obj, ev);
}

template<>
void AppWindow::toolButtonPressed
<PanelToolOption::NewParser>()
{
    newInstance();
}



AppWindow::ToolButtonsFunctions AppWindow::toolButtonsPressed =
{
    &AppWindow::toolButtonPressed<PanelToolOption::NewParser>,
};
