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

using namespace Panels::Configuration;

App::App(int argc, char** argv)
    : QApplication (argc, argv), appWindow(*this)
{
    setApplicationName("Tcl2Capl Parser");



}

App::~App(){

}



