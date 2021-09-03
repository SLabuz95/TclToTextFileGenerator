#include"appwindow.hpp"
#include"app.hpp"
#include<QTabBar>
#include<QEvent>
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"

AppWindow::AppWindow(App& app)
    : app(app),
      appModePanel(this),
      tcl2CaplFileParserPanel(*this),
      tcl2CaplInstructionParserPanel(*this)
{
    // Setup:
    // Check Initialization List    

    // - AppModePanel
    appModePanel.setDocumentMode(true);
    appModePanel.tabBar()->setExpanding(true);
    appModePanel.addTab(&tcl2CaplFileParserPanel, FILE_PARSER_TAB_TEXT);
    appModePanel.addTab(&tcl2CaplInstructionParserPanel, INSTRUCTION_PARSER_TAB_TEXT);

    setCentralWidget(&appModePanel);
    installEventFilter(this);

    show();
}

bool AppWindow::eventFilter(QObject *obj, QEvent *ev){
    if(obj == this and ev->type() == QEvent::Close){
        app.closeAllWindows();
    }
    return QMainWindow::eventFilter(obj, ev);
}
