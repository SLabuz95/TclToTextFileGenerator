#include"CANoeDBCFileReader.hpp"
//#include"FileReader/Reader/filereaderprivreadinfo.hpp"
//#include"FileReader/Interpreter/filereaderinterpreterpriv.hpp"
using CANoeDBC_Reader = FSD_CANoeDBC::Reader;

template<>
QString FileReader_CANoeDBC::fileFilters(){
    return QString("*.dbc");
}

template<>
bool FileReader_CANoeDBC::processing(){
 /*
  * 1) End of File Check
  * 2) Next Line
  * 3) Check For Errors
  */
    FileReader_Priv_ReadInfo* readInfo = nullptr;
    if(!(readInfo = readInfos.last()))  return ERROR_CALL(FSD_Error::ERROR);
    readInfo->addStats({FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::PROCESSING), realReadStat(CANoeDBC_Reader::Stat::CHECK_ERRORS), realReadStat(CANoeDBC_Reader::Stat::READ_NEXT_LINE), realReadStat(CANoeDBC_Reader::Stat::CHECK_END_OF_FILE)});
    return true;
}

template<>
bool FileReader_CANoeDBC::initialize(){
    QFile* file = nullptr;
    if(!(file = fileAccess()))
        return ERROR_CALL(FSD_Error::ERROR);
    return FileReader_Priv::initialize();
}

template<>
bool FileReader_CANoeDBC::deinitialize(){
    return FileReader_Priv::deinitialize();
}

template<>
template<>
bool FileReader_CANoeDBC::processingFunction<CANoeDBC_Reader::Stat::CHECK_END_OF_FILE>(){
    if(readInfos.last()->file.atEnd()){    // Clear all stats and set CLOSE_FILE and DEINIT Stats
        readInfos.last()->clearStats();
        readInfos.last()->addStats({FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::DEACTIVATE_COM), FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::DEINITIALIZE), FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::CLOSE_FILE),});
    }
    return true;
}

template<>
template<>
bool FileReader_CANoeDBC::processingFunction<CANoeDBC_Reader::Stat::READ_NEXT_LINE>(){
    QString line = readInfos.last()->file.readLine();
    line = line.trimmed();
    int stat = static_cast<int>(FSD_CANoeDBC::FileSpecificInterpreterStat::PROCESS_LINE);
    stat += static_cast<int>(FileReaderInterpreter_Priv_Stat::SIZE);
    NS_FRI_COM_Manager::ChannelCommand cmd = {NS_FRI_COM_Manager::Commands::EXEC_STAT, &stat};
    NS_FRI_COM_Manager::ChannelCommand cmdWithData = {NS_FRI_COM_Manager::Commands::DATA, &line};
    FRI_COM_Manager* com = FRI_COM_Manager::managerAccess();
    if(!com || !com->send(cmdWithData) || !processCOMMessage(cmd))
        return ERROR_CALL(FSD_Error::ERROR);
    return true;
}

template<>
template<>
bool FileReader_CANoeDBC::processingFunction<CANoeDBC_Reader::Stat::CHECK_ERRORS>(){
    return !(!interpreter.resultChecking() || errorCode());
}

template<>
QVector<ProcessingFunctions_FR<CANoeDBC_Reader>> FileReader<CANoeDBC_Reader>::fileReader_processingFunctions = {
    &FileReader_CANoeDBC::processingFunction<CANoeDBC_Reader::Stat::CHECK_ERRORS>,
    &FileReader_CANoeDBC::processingFunction<CANoeDBC_Reader::Stat::CHECK_END_OF_FILE>,
    &FileReader_CANoeDBC::processingFunction<CANoeDBC_Reader::Stat::READ_NEXT_LINE>
};

//template<>
//constexpr FSD_CANoeDBC::Reader::Error CompileTimeErrorControl<FSD_CANoeDBC::Reader::Error>::compileTimeError(){
//    return FSD_CANoeDBC::Reader::Error::NO_ERROR;
//}
