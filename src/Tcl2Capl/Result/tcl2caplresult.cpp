#include"tcl2caplresult.hpp"

Tcl2CaplResult::Tcl2CaplReadData::InitializeStatus Tcl2CaplResult::Tcl2CaplReadData::initialize(){
    if(initializeStatus == InitializeStatus::NOT_INITIALIZED){
        if(userConfig_.proceduresSettings().isWriteToFileMode()){
            _file.setFileName(_dir.path() + "/" + _dir.dirName() + ".can");
            return initializeStatus = InitializeStatus::INITIALIZE_SUCCESS;
        }else{
            return initializeStatus;
        }
    }
    return InitializeStatus::ALREADY_INITIALIZED;
}

Tcl2CaplResult& Tcl2CaplResult::Tcl2CaplReadData::generateResult(){
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
