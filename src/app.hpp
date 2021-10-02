#ifndef APP_HPP
#define APP_HPP


#include<QApplication>
//#include"Tcl2Capl/Tcl2Capl.hpp"
#include"appwindow.hpp"
#include<QTableWidget>

#include"StartUpError/startuperror.hpp"
#include"Tcl2CaplPanels/ConfigEditor/configEditor.hpp"
#include"Tcl2Capl/controllerconfigmanager.hpp"

class TestModule;
class App : public QApplication{
public:
    App(int argc, char** argv);
    ~App();

    // Parser (Thread)
    //Tcl2Capl tcl2Capl;

    // GUI
    AppWindow appWindow;
    //ConfigEditor configEditor;
    ControllerConfigManager configManager_;
    //Tcl2CaplControllerConfig config;

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
