#ifndef CONFIGEDITOR_HPP
#define CONFIGEDITOR_HPP

#include<QBoxLayout>
#include<QTabWidget>
#include<QListWidget>
#include<QTreeWidget>
#include<QToolBar>
#include<QPushButton>
#include<QApplication>
#include<QMouseEvent>
#include<QSplitter>
#include<QLabel>
#include<QToolBox>
#include<QTableWidget>
#include<QTableWidgetItem>
#include<QComboBox>
#include<QLineEdit>
#include<QHeaderView>
#include"TclInterpreter/tclToCAPL.hpp"
#include<QFormLayout>
#include"writeOnlyProceduresList.hpp"
#include"proceduresList.hpp"
#include"rulesprocedurepanel.hpp"
#include"fileconfigpanel.hpp"
#include"loadconfigsettings.hpp"
#include"Tcl2Capl/controllerconfiginfo.hpp"
#include"Tcl2CaplPanels/ConfigEditor/ConfigViewPanel/configViewPanel.hpp"

class App;

namespace Panels::Configuration{
    using namespace Navigation;
    class Panel : public QWidget{
    public:
        using NavigationList = Navigation::List;
        using Config = Tcl2CaplControllerConfig;
        using ConfigRef = Config&;
        using ConfigPtr = Config*;
        using ConfigInfoPtr = ControllerConfigInfo::SelfPtr;

        Panel(App&);
        virtual ~Panel() override;

    protected:
        using Layout = QVBoxLayout;

        using RulesView = QTreeWidget;
        using NoSelectedProcedurePanel =  QLabel;

        App& app_;

        Layout layout;
        FileConfigPanel fileConfigPanel;
        View::ConfigViewPanel configViewPanel;
        // NO GUI ELEMENT
        ConfigInfoPtr configInfoPtr = nullptr;
        // ---------------------
        //QToolBox toolBox;
        //WriteOnlyProceduresList writeOnlyProceduresList;
        //ProceduresList proceduresList;
        //RulesProcedurePanel rulesProcedurePanel;
        //NoSelectedProcedurePanel noSelectedProcedurePanel;


        bool newConfig();
        bool readConfig(QString);
        bool saveConfig(QString);
        void loadConfigData(ConfigInfoPtr, LoadConfigSettings);
        void reloadGui();

        void loadProcedureRulesPanel();
        void clearProcedureRulesPanel();

        bool eventFilter(QObject*, QEvent*) override;

    public:
        inline ConfigInfoPtr getConfigInfoPtr()const{return configInfoPtr;}
        inline bool request_newConfig(){return newConfig();}
        inline bool request_readConfig(QString path){return readConfig(path);}
        inline bool request_saveConfig(QString path){return saveConfig(path);}
        inline bool anyChanges(){
            //return writeOnlyProceduresList.anyChanges() or
              //      proceduresList.anyChanges();
        }

         bool isDefaultConfig();

        inline void reloadGuiForUpdatedConfig(){
            //writeOnlyProceduresList.reloadGui();
            //proceduresList.reloadGui();
            //reloadGui();
        }

        inline App& app()const{return app_;}


    };

};

#endif // CONFIGEDITOR_HPP
