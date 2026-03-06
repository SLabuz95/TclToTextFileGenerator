#include"app.hpp"
#include"External/StartUpError/startuperror.hpp"

#include<QApplication>

using namespace Panels::Configuration;

App::App(int argc, char** argv)
    : QApplication (argc, argv),  appWindow(*this)
{
    setApplicationName("Tcl2Capl Parser");
    /*QPalette pallete = qApp->palette();
    QFile f( ":qdarkstyle/light/lightstyle.qss" );
    if ( !f.exists() )
    {
        qDebug() << QFileInfo(f).absoluteFilePath();
       qWarning() << "Unable to set dark stylesheet, file not found";
    }
    else
    {
       f.open( QFile::ReadOnly | QFile::Text );
       QTextStream ts( &f );
       qApp->setStyleSheet( ts.readAll() );
    }*/


}

App::~App(){

}



