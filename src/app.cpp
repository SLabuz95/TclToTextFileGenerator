#include"app.hpp"
#include"External/StartUpError/startuperror.hpp"
#include<QFileDialog>
#include<QDirIterator>
#include"TcFileModifier/tcfilemodifier.hpp"
#include<QTabWidget>
#include<QLabel>
#include<QLibrary>
#include<QTableWidgetItem>
#include<QStyleFactory>

App::App(int argc, char** argv)
    : QApplication (argc, argv), appWindow(*this)
{
    setApplicationName("Tcl2Capl Parser");

    (new ConfigEditor(*this))->show();

}

App::~App(){

}



