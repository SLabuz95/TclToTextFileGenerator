#ifndef CONTROLLER_H
#define CONTROLLER_H
//#include"Result/tcl2caplresult.hpp"
#include<QDir>
#include"controllerconfig.hpp"
//#include"Tcl2Capl/caplFunctionDefiniitions.hpp"
#include<QThread>


class UserInputConfig{
public:
    using TclProcedureInterpreter = Tcl::Interpreter::CommandsController::Controller;
    using Settings = TclProcedureInterpreter::ProdecuresSettings;
    using WriteOnlyProcedures = Settings::WriteOnlyProcedures;
    using UserProcedure = Tcl::Interpreter::CommandsController::Definition;
    using UserProcedures = Tcl::Interpreter::CommandsController::CommandDefinitions;
    using UserDefaultProcedure = UserProcedure;
    using Predefinitions = Tcl::Interpreter::PredefinitionsController::Predefinitions;

    using UserInputConfigData = Tcl2CaplControllerConfig;
    UserInputConfig(){}
    UserInputConfig(UserInputConfigData& configData);
    ~UserInputConfig(){}
protected:
    Settings settings_;
    UserProcedures userProcedures_;
    UserDefaultProcedure userDefaultProcedure_;
    Predefinitions _predefinitions;

public:
    inline Settings& proceduresSettings(){return settings_;}
    inline UserProcedures& userProcedureConfig(){return userProcedures_;}
    inline UserDefaultProcedure& userDefaultProcedureConfig(){return userDefaultProcedure_;}
    inline Predefinitions& predefinitions(){return _predefinitions;}
};

class FileInstanceResultPanel;
class Tcl2CaplResult;
class Tcl2CaplController : public QThread{
    // Concept Definition
public:
    using TclProcedureInterpreter = Tcl::Interpreter::CommandsController::Controller;
    using Settings = TclProcedureInterpreter::ProdecuresSettings;
    using WriteOnlyProcedures = Settings::WriteOnlyProcedures;
    using UserProcedure = Tcl::Interpreter::CommandsController::Definition;
    using UserProcedures = Tcl::Interpreter::CommandsController::CommandDefinitions;
    using UserDefaultProcedure = UserProcedure;
    using Predefinitions = Tcl::Interpreter::PredefinitionsController::Predefinitions;

    using Error = QString;
    using Results = QVector<Tcl2CaplResult*>;
    using ReportFile = QFile;
    using CAPLDefinitionsFile = QFile;

    // ------------------
private:
    // Properties -------------
    const QString fileSeparator = QStringLiteral("\\");
    const QString reportFileName = QStringLiteral("errors.html");
    const QString caplDefinitionsFileName = QStringLiteral("functions.can");

    //QDir inputDir;
    QStringList definitionPaths_;
    QStringList inputPaths_;
    QDir outputDir;
    QString userOutputDir;
    //QString configFilePath_;
    QObject* progressEventDest = nullptr;

    UserInputConfig userInputConfig_;

    Predefinitions predefinitions;

    Error _error;
    Results results;
    ReportFile reportFile;
    CAPLDefinitionsFile caplDefinitionsFile;
    // ------------------------

    // Functions --------------
    inline Error throwError(Error error){return _error = error;}

    inline void setReportFilePath(QDir dir){reportFile.setFileName(dir.path() + fileSeparator + reportFileName);}
    inline void setCaplDefinitionsFilePath(QDir dir){caplDefinitionsFile.setFileName(dir.path() + fileSeparator + caplDefinitionsFileName);}
    inline void addResult(Tcl2CaplResult& newResult){results.append(&newResult);}
    //Error readProceduresConfig(QString&);
    // -----------------------

    // Thread Main Function
    void run() override;
    bool generateDefinitions_impl();
    bool generateCapls_impl();
    bool writeResults();

    // Interface -------------
public:
    Tcl2CaplController(QStringList& definitions, QStringList& inputPath, QString& outputPath);

    ~Tcl2CaplController();

    inline void quitAndWait(){quit(); wait();}
    inline bool isError()const{return not _error.isEmpty();}
    inline Error error()const{return _error;}
    inline void clearError(){_error.clear();}
    //inline QDir getInputDir(){return inputDir;}

    //const QString inputDirPath()const{return inputDir.path();}
    inline QStringList& definitionPaths(){return definitionPaths_;}
    inline QStringList& inputPaths(){return inputPaths_;}
    const QString outputDirPath()const{return outputDir.path();}
    //const QString& configFilePath()const{return configFilePath_;}
    const QString reportFilePath()const{return reportFile.fileName();}

    void prepareOutputDirs(QString output);
    //Error readNewInputConfig();
    inline void generateCaplsFromFolderWithTcls(QObject* setProgressEventDest, UserInputConfig::UserInputConfigData& config){
        progressEventDest = setProgressEventDest;
        userInputConfig_ = config;
        start();
    };
    inline void generateCaplsFromFolderWithTcls(QObject* setProgressEventDest, UserInputConfig::UserInputConfigData& config, UserInputConfig::Settings::InterpreterMode mode){
        progressEventDest = setProgressEventDest;
        userInputConfig_ = config;
        userInputConfig_.proceduresSettings().setMode(mode);
        start();
    };
    inline UserInputConfig& userProceduresConfig(){return userInputConfig_;}
};



#endif // CONTROLLER_H
