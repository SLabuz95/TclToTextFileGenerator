#include"app.hpp"
#include"StartUpError/startuperror.hpp"
#include<QFileDialog>
#include<QDirIterator>
#include"TcFileModifier/tcfilemodifier.hpp"
#include<QTabWidget>
#include<QLabel>
#include<QLibrary>
#include<QTableWidgetItem>

App::App(int argc, char** argv)
    : QApplication (argc, argv), appWindow(*this)
{
    setApplicationName("Tcl2Capl Parser");
    setOrganizationName("APTIV");

    (new ConfigEditor(*this))->show();

}

App::~App(){
    clearMemory();
}

void App::clearMemory(){
}


