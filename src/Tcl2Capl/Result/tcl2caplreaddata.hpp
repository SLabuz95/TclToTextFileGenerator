#ifndef TCL2CAPLREADDATA_HPP
#define TCL2CAPLREADDATA_HPP

#include"External/FileReader/FilesSpecificData/FSDTemplate.hpp"
#include<QDir>
#include"TclInterpreter/tclToCAPL.hpp"
#include"Tcl2Capl/caplFunctionDefiniitions.hpp"

using namespace Tcl::Analysis;
class Tcl2CaplResult;
class UserInputConfig;
class Tcl2CaplReadData : public FSD_DataModelBase<Tcl2CaplReadData>{
    using FormatCount = QMap<QString, uint>;
    using TestCaseErrors = uint;
    using Predefinitions = Tcl::Interpreter::PredefinitionsController::Predefinitions;
    using Error = QString;
    using ScriptFile = QFile;
protected:
    enum class _InitializeStatus{
        NOT_INITIALIZED,
        INITIALIZE_SUCCESS,
        INITIALIZE_FAIL,
        ALREADY_INITIALIZED
    };

    QDir _dir;
    ScriptFile _file;
    QFile& _reportFile;
    QString _currentTCLFileName;
    QString _fileContent;
    _InitializeStatus initializeStatus = _InitializeStatus::NOT_INITIALIZED;
    UserInputConfig& userConfig_;
    FunctionDefinitionsRef caplFunctionDefinitionsRef_;
    TestCaseErrors testCaseErrors_ = 0;

    //inline QFile& file(){return _file;}
public:
    using InitializeStatus = _InitializeStatus;
    Tcl2CaplReadData(QDir outputDir, QFile& reportFile, UserInputConfig& userConfig, FunctionDefinitionsRef caplFunctionDefinitionsRef)
        : _dir(outputDir), _reportFile(reportFile), userConfig_(userConfig), caplFunctionDefinitionsRef_(caplFunctionDefinitionsRef){}
    ~Tcl2CaplReadData(){
        _file.close();
    }
    inline QDir& dir(){return _dir;}
    InitializeStatus initialize();
    inline void write(QString str){_fileContent.append(str);}
    inline QFile& reportFile(){return _reportFile;}
    inline void setCurrentTCLFileName(QString str){_currentTCLFileName = str;}
    inline QString currentTCLFileName(){return _currentTCLFileName;}
    inline bool isInitiliazed(){return initializeStatus != InitializeStatus::NOT_INITIALIZED;}
    Tcl2CaplResult& generateResult();
    inline UserInputConfig& userConfig(){return userConfig_;}
    inline FunctionDefinitionsRef caplFunctionDefinitionsRef(){return caplFunctionDefinitionsRef_;}
    inline TestCaseErrors testCaseErrors()const{return testCaseErrors_;}
    inline void setTestCaseErrors(TestCaseErrors number){testCaseErrors_ = number;}
    Predefinitions& predefinitions();
};

#endif // TCL2CAPLREADDATA_HPP
