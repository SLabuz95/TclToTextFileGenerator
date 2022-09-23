#include"tcl2caplreaddata.hpp"
#include"tcl2caplresult.hpp"
#include"Tcl2Capl/controller.hpp"

using Predefinitions = Tcl::Interpreter::PredefinitionsController::Predefinitions;
Tcl2CaplReadData::InitializeStatus Tcl2CaplReadData::initialize(){
    if(initializeStatus == InitializeStatus::NOT_INITIALIZED){
        if(userConfig_.proceduresSettings().isWriteToFileMode()){
            switch(_readMode){
            case ReadMode::FileMode:
                _file.setFileName(_dir.path() + "/" + _currentTCLFileName.remove(".tc") + ".can");
                break;
            case ReadMode::DirectoryMode:
                _file.setFileName(_dir.path() + "/" + _dir.dirName() + ".can");
                break;
            }

            return initializeStatus = InitializeStatus::INITIALIZE_SUCCESS;
        }else{
            return initializeStatus;
        }
    }
    return InitializeStatus::ALREADY_INITIALIZED;
}


Predefinitions& Tcl2CaplReadData::predefinitions()
{
    return userConfig_.predefinitions();
}

Tcl2CaplResult& Tcl2CaplReadData::generateResult(){
    Tcl2CaplResult& tcl2CaplResult = *(new Tcl2CaplResult(_file, _fileContent));

    return tcl2CaplResult;

}


Tcl2CaplResult::Error Tcl2CaplResult::writeResult(){
    if(!QFileInfo(_scriptFile).dir().exists())
        return throwError(QString("Directory Path: ") + QFileInfo(_scriptFile).dir().path() + " isnt exist.");
    if(_scriptFile.isOpen())
        return throwError(QString("File: ") + _scriptFile.fileName() + " is already open.");
    if(!_scriptFile.open(QIODevice::Text | QIODevice::WriteOnly))
        return throwError(QString("Open File Error: ") + _scriptFile.fileName());

    // Write to file
    _scriptFile.write(_script.content().toUtf8());

    _scriptFile.close();
    return QString();
}
