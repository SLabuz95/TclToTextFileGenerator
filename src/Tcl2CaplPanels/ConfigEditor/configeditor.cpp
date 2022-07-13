#include"configEditor.hpp"
#include<QHeaderView>
#include<QMimeData>
#include<External/RegExpCore/regexprcore.hpp>
#include"Tcl2Capl/controllerconfigmanager.hpp"
#include"app.hpp"
#include"ConfigTabsPanel/Panels/attributesPanel.hpp"
#include<QSplitterHandle>
#include<QPainter>
#include<QEnterEvent>

using namespace Panels::Configuration;

Panel::Panel(App& app)
    : app_(app),
      fileConfigPanel(*this),
      configViewPanel(*this)
{
    const QString NO_SELECTED_PROCEDURE_PANEL_TEXT = QStringLiteral("Wybierz procedure");
//    noSelectedProcedurePanel.setText(NO_SELECTED_PROCEDURE_PANEL_TEXT);
//    noSelectedProcedurePanel.setAlignment(Qt::AlignCenter);

    layout.setSpacing(0);
    layout.setContentsMargins(0, 0, 0, 0);
    layout.addWidget(&fileConfigPanel, 1, Qt::AlignTop);
    layout.addWidget(&configViewPanel, Qt::AlignTop);
    installEventFilter(this);

    setLayout(&layout);

    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    configTabsPanel.rulesProcedureList().setEnabled(false);
    configTabsPanel.rulesDefaultProcedureList().setEnabled(false);

    // Register temporary config (config manager - register without path)
    // if success, load new config info ptr
    // if failed, close editor
    /*ConfigInfoPtr configInfoPtr = app.configManager().loadConfig(this);
    if(configInfoPtr != nullptr){
        loadConfigData(configInfoPtr, LoadConfigSettings::LoadGui);
    }else{
        close();
    }*/
}

bool Panel::isDefaultConfig(){
   //Q_ASSERT_X(app().configManager().isInfoExist(configInfoPtr), "Panel::isDefaultConfig", "Internal error: ConfingInfo is not registered");
   //return configInfoPtr->isDefaultConfig();
}

Panel::~Panel(){
    /*if(configInfoPtr){
        if(app().configManager().unloadConfig(configInfoPtr, this) == false){
            qDebug() << "ConfigManager::unregister Error: " + app().configManager().lastErrorMessage();
        }
    }
    configInfoPtr = nullptr;*/
}

bool Panel::eventFilter(QObject* obj, QEvent* ev){
    if(obj == this and ev->type() == QEvent::Close){
        delete obj;
        return true;
    }
    return QWidget::eventFilter(obj, ev);
}
void Panel::syncConfig(){
    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    if(configTabsPanel.rulesProcedureList().initialized()){
        ConfigInfo::Config::DynamicRawRules newRules;
        configTabsPanel.rulesProcedureList().readRules(newRules);
        config().loadNewRules(configTabsPanel.rulesProcedureList().procedureName, configTabsPanel.rulesProcedureList().rulesCategory, newRules);
    }
    if(configTabsPanel.rulesDefaultProcedureList().initialized()){
        ConfigInfo::Config::DynamicRawRules newRules;
        configTabsPanel.rulesDefaultProcedureList().readRules(newRules);
        config().loadNewRules(configTabsPanel.rulesDefaultProcedureList().rulesCategory, newRules);
    }
}
void Panel::loadRules(QString procedureName, QString rulesCategory){
    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    ConfigInfo::RulesCategories category = static_cast<ConfigInfo::RulesCategories>(rulesCategory.toLongLong());
    if(configTabsPanel.rulesProcedureList().procedureName != procedureName or configTabsPanel.rulesProcedureList().rulesCategory != category){
        if(configTabsPanel.rulesProcedureList().initialized()){
            ConfigInfo::Config::DynamicRawRules newRules;
            configTabsPanel.rulesProcedureList().readRules(newRules);
            config().loadNewRules(configTabsPanel.rulesProcedureList().procedureName, configTabsPanel.rulesProcedureList().rulesCategory, newRules);
        }
        ConfigInfo::RulesView rulesView = config().readRules(procedureName, category);
        configTabsPanel.rulesProcedureList().setEnabled(true);
        configTabsPanel.rulesProcedureList().procedureName = procedureName;
        configTabsPanel.rulesProcedureList().rulesCategory = category;
        configTabsPanel.rulesProcedureList().loadRules(rulesView);
    }

}

void Panel::loadDefaultRules(QString rulesCategory){
    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    ConfigInfo::RulesCategories category = static_cast<ConfigInfo::RulesCategories>(rulesCategory.toLongLong());
    if(configTabsPanel.rulesDefaultProcedureList().rulesCategory != category){
        if(configTabsPanel.rulesDefaultProcedureList().initialized()){
            ConfigInfo::Config::DynamicRawRules newRules;
            configTabsPanel.rulesDefaultProcedureList().readRules(newRules);
            config().loadNewRules(configTabsPanel.rulesDefaultProcedureList().rulesCategory, newRules);
        }
        ConfigInfo::RulesView rulesView = config().readRules(category);
        configTabsPanel.rulesDefaultProcedureList().setEnabled(true);
        configTabsPanel.rulesDefaultProcedureList().rulesCategory = category;
        configTabsPanel.rulesDefaultProcedureList().loadRules(rulesView);
    }
}

bool Panel::editProcedure(QString oldName, QString newName){
    if(config().editProcedureName(oldName, newName) == true){
        View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
        if(configTabsPanel.rulesProcedureList().procedureName == oldName)
            configTabsPanel.rulesProcedureList().procedureName = newName;
        return true;
    }
    return false;
}

bool Panel::editIndex(qsizetype oldIndex, qsizetype newIndex){
    ConfigInfo::RulesCategories oldCategory = static_cast<ConfigInfo::RulesCategories>(oldIndex);
    ConfigInfo::RulesCategories newCategory = static_cast<ConfigInfo::RulesCategories>(newIndex);
    if(config().editIndex(oldIndex, newIndex) == true){
        View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
        if(configTabsPanel.rulesDefaultProcedureList().rulesCategory == oldCategory)
            configTabsPanel.rulesDefaultProcedureList().rulesCategory = newCategory;
        return true;
    }
    return false;
}
bool Panel::editIndex(QString procedureName, qsizetype oldIndex, qsizetype newIndex){
    ConfigInfo::RulesCategories oldCategory = static_cast<ConfigInfo::RulesCategories>(oldIndex);
    ConfigInfo::RulesCategories newCategory = static_cast<ConfigInfo::RulesCategories>(newIndex);
    if(config().editIndex(procedureName, oldIndex, newIndex) == true){
        View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
        if(configTabsPanel.rulesProcedureList().procedureName == procedureName
                and configTabsPanel.rulesProcedureList().rulesCategory == oldCategory)
        {
            configTabsPanel.rulesProcedureList().rulesCategory = newCategory;
        }
        return true;
    }
    return false;
}

void Panel::removeProcedure(QString procedureName){
    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    config().removeProcedure(procedureName);
    if(configTabsPanel.rulesProcedureList().procedureName == procedureName){
        configTabsPanel.rulesProcedureList().setNonInitialized();
        configTabsPanel.rulesProcedureList().clear();
        configTabsPanel.rulesProcedureList().setEnabled(false);
    }
}

void Panel::removeIndex(QString procedureName, qsizetype index){
    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    config().removeIndex(procedureName, index);
    ConfigInfo::RulesCategories category = static_cast<ConfigInfo::RulesCategories>(index);
    if(configTabsPanel.rulesProcedureList().procedureName == procedureName
            and configTabsPanel.rulesProcedureList().rulesCategory == category)
    {
        configTabsPanel.rulesProcedureList().setNonInitialized();
        configTabsPanel.rulesProcedureList().clear();
        configTabsPanel.rulesProcedureList().setEnabled(false);
    }
}

void Panel::removeIndex(qsizetype index){
    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    config().removeIndex(index);
    ConfigInfo::RulesCategories category = static_cast<ConfigInfo::RulesCategories>(index);
    if(configTabsPanel.rulesDefaultProcedureList().rulesCategory == category)
    {
        configTabsPanel.rulesDefaultProcedureList().setNonInitialized();
        configTabsPanel.rulesDefaultProcedureList().clear();
        configTabsPanel.rulesDefaultProcedureList().setEnabled(false);
    }
}

void Panel::clearProcedures(){
    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    config().clearProcedures();
    configTabsPanel.rulesProcedureList().setNonInitialized();
    configTabsPanel.rulesProcedureList().clear();
    configTabsPanel.rulesProcedureList().setEnabled(false);
}

void Panel::clearIndexes(QString procedureName){
    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    config().clearIndexes(procedureName);
    if(configTabsPanel.rulesProcedureList().procedureName == procedureName)
    {
        configTabsPanel.rulesProcedureList().setNonInitialized();
        configTabsPanel.rulesProcedureList().clear();
        configTabsPanel.rulesProcedureList().setEnabled(false);
    }
}

void Panel::clearIndexes(){ // DefaultProcedure
    View::ConfigTabs& configTabsPanel = configViewPanel.Panels::Super::ViewPanel::Super::get();
    config().clearIndexes();
    configTabsPanel.rulesDefaultProcedureList().setNonInitialized();
    configTabsPanel.rulesDefaultProcedureList().clear();
    configTabsPanel.rulesDefaultProcedureList().setEnabled(false);
}

bool Panel::newConfig(){
    // Check if configInfo still exists
    /*Q_ASSERT_X(app().configManager().isInfoExist(configInfoPtr), "Panel::newConfig", "Internal error: ConfingInfo is not registered");

    bool errorOccurred = false;

    {   // Ask to save to file
        bool saveToFile = false;

        if(configInfoPtr->isTemporaryConfiguration()){  // Temporary Config
            // Ask to save to file
            if(anyChanges() or //  On Changes
                 not configInfoPtr->isDefaultConfig()){   // On not default configuration without changes
                saveToFile = true;
            }
        }else{  // File Config
            if(anyChanges()) //  On Changes
                saveToFile = true;
        }

        // If saveToFile is satisfied (true)
        if(saveToFile == true){
            // Dialog with save to file question
            // If yes, call save to file from fileManager

        }
    }

    if(not errorOccurred){ // Change config with new temporary config
        ConfigInfoPtr newConfigInfo = nullptr;
        newConfigInfo = app().configManager().changeConfig(configInfoPtr, QString());
        if(newConfigInfo != nullptr){   // Normal case (no errors)
            // Load Config and Reload Gui
            loadConfigData(newConfigInfo, LoadConfigSettings::LoadGui);
        }else{
            errorOccurred = true;
        }
    }

    return not errorOccurred;*/
}
/*
void Panel::loadConfigData(ConfigInfoPtr configInfo, LoadConfigSettings settings){
    Q_ASSERT_X(configInfo != nullptr, "Panel::loadConfig", "ConfigInfo is null");
    if(configInfo != configInfoPtr){    // If
        configInfoPtr = configInfo;
        configViewPanel.loadConfigData(settings);
        if(settings == LoadConfigSettings::LoadGui){
            // Load Gui
            reloadGuiForUpdatedConfig();
        }
    }
}
*/
void Panel::reloadGui(){

}

bool Panel::saveConfig(QString path){
    bool errorOccurred = false;

    /*if(app().configManager().saveConfig(configInfoPtr, path) == nullptr){    // Error
        errorOccurred = true;
    }*/

    return not errorOccurred;
}


bool Panel::readConfig(QString path)
{
    bool errorOccurred = false;

    // If file exists, reregister
    /*ConfigInfoPtr newConfigInfo = nullptr;
    newConfigInfo = app().configManager().changeConfig(configInfoPtr, path);
    if(newConfigInfo != nullptr){   // Register Success
        loadConfigData(newConfigInfo, LoadConfigSettings::LoadGui);
    }else{
        errorOccurred = true;
    }*/

    return not errorOccurred;
}

void Panel::clearProcedureRulesPanel(){
//    splitter.replaceWidget(1, &noSelectedProcedurePanel);
}
