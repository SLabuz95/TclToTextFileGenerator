#ifndef TCL2CAPLRESULT_HPP
#define TCL2CAPLRESULT_HPP

#include<QDir>
#include"External/FileReader/FilesSpecificData/FSDTemplate.hpp"
#include"tclToCAPL.hpp"
#include"Tcl2Capl/controller.hpp"

class Tcl2CaplResult{
    // Concept Definition

    class Script{
        QString _content;

    public:
        Script(QString& content) : _content(content){}
        inline QString& content(){return _content;}
    };
    // ====
public:
    using Error = QString;
    using ScriptFile = QFile;

    class Tcl2CaplReadData : public FSD_DataModelBase<Tcl2CaplReadData>{
        using FormatCount = QMap<QString, uint>;
        using TestCaseErrors = uint;
        using Predefinitions = TCLInterpreterPriv::PredefinitionsControl::Predefinitions;
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
        inline void write(QString str){_fileContent.append(str);};
        inline QFile& reportFile(){return _reportFile;}
        inline void setCurrentTCLFileName(QString str){_currentTCLFileName = str;}
        inline QString currentTCLFileName(){return _currentTCLFileName;}
        inline bool isInitiliazed(){return initializeStatus != InitializeStatus::NOT_INITIALIZED;}
        Tcl2CaplResult& generateResult();
        inline UserInputConfig& userConfig(){return userConfig_;}
        inline FunctionDefinitionsRef caplFunctionDefinitionsRef(){return caplFunctionDefinitionsRef_;}
        inline TestCaseErrors testCaseErrors()const{return testCaseErrors_;}
        inline void setTestCaseErrors(TestCaseErrors number){testCaseErrors_ = number;}
        Predefinitions& predefinitions(){return userConfig_.predefinitions();}
    };
private:

    // ------------------

    // Properties -------------
    Error _error;
    ScriptFile _scriptFile;
    Script _script;
    // ------------------------

    // Functions --------------
    inline Error throwError(Error error){return _error = error;}

    // -----------------------

    // Interface -------------
public:
    Tcl2CaplResult(ScriptFile& scriptFile, Script script)
        : _script(script)
    {
        _scriptFile.setFileName(scriptFile.fileName());
    }

    inline bool isError()const{return !_error.isEmpty();}
    inline Error error()const{return _error;}

    Error writeResult();
};

#endif // TCL2CAPLRESULT_HPP
