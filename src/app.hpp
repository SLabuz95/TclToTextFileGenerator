#ifndef APP_HPP
#define APP_HPP


#include<QApplication>
//#include"Tcl2Capl/Tcl2Capl.hpp"
//#include"appwindow.hpp"
#include"Tcl2CaplPanels/MainApp/mainApp.hpp"
#include<QTableWidget>

#include"External/StartUpError/startuperror.hpp"
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"
#include"Tcl2Capl/controllerconfigmanager.hpp"

using namespace Panels::Configuration;
class TestModule;
class App : public QApplication{
    using AppWindow = MainWindow;
public:
    App(int argc, char** argv);
    ~App();

    // GUI
    AppWindow appWindow;
    ControllerConfigManager configManager_;

    void clearMemory();

    inline ControllerConfigManager& configManager(){return configManager_;}

    template<typename ...startupClass>
    bool startupErrorChecking(){
        bool ret = ( ... && StartUpError<startupClass>::__startupErrorChecking());
        if(!ret){
            Q_ASSERT_X(false, "Startup", "On StartUp");
        }
        return ret;
    }

};

#endif // APP_HPP
