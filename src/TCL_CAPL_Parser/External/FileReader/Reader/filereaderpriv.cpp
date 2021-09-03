#include"filereaderpriv.hpp"
#include"FileReader/Interpreter/filereaderinterpreterpriv.hpp"
#include"filereaderprivreadinfo.hpp"
#include"FileReader/Interpreter/filereaderinterpreterprivreadinfo.hpp"
#include<QFileDialog>
#include<QDebug>
#include<QMutexLocker>

QVarLengthArray<ProcessingFunctions_FR_Priv, FileReader_Priv::numbOfReadStats()> FileReader_Priv::fileReaderPriv_processingFunctions = {
    &FileReader_Priv::noStatReturned,
    &FileReader_Priv::processing,
    &FileReader_Priv::initialize,
    &FileReader_Priv::deinitialize,
    &FileReader_Priv::openFile,
    &FileReader_Priv::closeFile,
    //&FileReader_Priv::activateCOM,
    //&FileReader_Priv::deactivateCOM,
        };

FileReader_Priv::FileReader_Priv(FileReaderInterpreter_Priv& interpreter)
    : COMChannel(interpreter.COMChannel),interpreter(interpreter)
{}

QString FileReader_Priv::readFile(QString dialogDescriptor, QString fileFilter, QStringList blackList = QStringList()){
    if(fileFilter.isEmpty())
        fileFilter = fileFilters();
    // Ask for file
    QString fileName = QFileDialog::getOpenFileName(nullptr, dialogDescriptor, QDir::homePath(), fileFilter);
    if(fileName.isEmpty()){
        //setError( XML_READ_OPEN_FILE_ABORTED);
        return QString();
    }
    return readFileByFilePath(QFileInfo(fileName), fileFilter, blackList);
}

QString FileReader_Priv::readFileByFilePath(QFileInfo&& fileInfo, QString fileFilter, QStringList blackList = QStringList()){
    if(fileFilter.isEmpty())
        fileFilter = fileFilters();
    using Filters = QStringList;
    using Filter = Filters::Iterator;
    Filters filters = fileFilter.split(";");
    Filter filter = filters.begin();
    for( ; filter < filters.end(); filter++){
        if(fileInfo.suffix() == QFileInfo(*filter).suffix())
            break;
    }
    if(filter == filters.end())
        return fileInfo.filePath();
    if( not blackList.contains(fileInfo.filePath())){
        readInfo.setFileName(fileInfo.filePath());
        while(exec());
        resultChecking();
    }else{
        onTheBlackList = true;
    }
    return fileInfo.filePath();
}

bool FileReader_Priv::exec(){
    return (this->*fileReaderPriv_processingFunctions.at(readInfo.takeLastStat()))();
}

bool FileReader_Priv::resultChecking(){
    readInfo.clearStats();
    readInfo.closeFile();
}

bool FileReader_Priv::openFile(){
    qDebug() << readInfo.getFile().fileName();
    if(readInfo.openFile()){
        qDebug() << readInfo.getFile().fileName() << "Open Event";
    }else{
        if(not readInfo.getFile().exists()){
            setErrorType(Error::FileDoesntExistError);
        }else{
            setErrorType(Error::FileOpenError);
        }
    }
    return isNoError();
}

QFile* FileReader_Priv::fileAccess(){
    return &readInfo.getFile();
}

bool FileReader_Priv::closeFile(){
    readInfo.closeFile();
    qDebug() << readInfo.getFile().fileName() << "Close Event";
    return isNoError();
}

bool FileReader_Priv::initialize(){
    FileReaderInterpreter_Priv_Stat interpreterStat = FileReaderInterpreter_Priv_Stat::INITIALIZE;
    QString filePath = readInfo.getFile().fileName();
    NS_FRI_COM_Manager::ChannelDatas cmds = {
        {NS_FRI_COM_Manager::Commands::EXEC_STAT, &interpreterStat},
        {NS_FRI_COM_Manager::Commands::DATA, &filePath}
    };
    qDebug() << readInfo.getFile().fileName() << "Init Event";

    return processCOMMessage(cmds);
}

bool FileReader_Priv::deinitialize(){
    // Interpreter and Interpreter COnfig deinitialization in Interpreter Config Destructor
    readInfo.clearStats();
    qDebug() << readInfo.getFile().fileName() << "Deinit Event";
    return isNoError();
}

bool FileReader_Priv::processCOMMessage(NS_FRI_COM_Manager::ChannelDatas &cmds){
    FRI_COM_Manager::send(COMChannel, cmds);
    ;
    return interpreter.process();
    //COM->last().commands.clear();
}
