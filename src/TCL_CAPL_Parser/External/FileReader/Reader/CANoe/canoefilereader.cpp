#include"CANoeFileReader.hpp"
//#include"FileReader/Reader/filereaderprivreadinfo.hpp"
//#include"FileReader/Interpreter/filereaderinterpreterpriv.hpp"
using CANoe_Reader = FSD_CANoe::Reader;

template<>
QString FileReader_CANoe::fileFilters(){
    return QString();
}

template<>
bool FileReader_CANoe::processing(){
 /*
  * 1) End of File Check
  * 2) Next Line
  * 3) Check For Errors
  */
    FileReader_Priv_ReadInfo* readInfo = nullptr;
    if(!(readInfo = readInfos.last()))  return ERROR_CALL(FSD_Error::ERROR);
    readInfo->addStats({FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::PROCESSING), realReadStat(CANoe_Reader::Stat::CHECK_ERRORS), realReadStat(CANoe_Reader::Stat::READ_NEXT_LINE), realReadStat(CANoe_Reader::Stat::CHECK_END_OF_FILE)});
    return true;
}

template<>
bool FileReader_CANoe::initialize(){
    QFile* file = nullptr;    
    if(!(file = fileAccess()))
        return ERROR_CALL(FSD_Error::ERROR);
    return FileReader_Priv::initialize();
}

template<>
bool FileReader_CANoe::deinitialize(){
    return FileReader_Priv::deinitialize();
}

template<>
template<>
bool FileReader_CANoe::processingFunction<CANoe_Reader::Stat::CHECK_END_OF_FILE>(){
    if(readInfos.last()->file.atEnd()){    // Clear all stats and set CLOSE_FILE and DEINIT Stats
        readInfos.last()->clearStats();
        readInfos.last()->addStats({FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::DEACTIVATE_COM), FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::DEINITIALIZE), FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::CLOSE_FILE),});
    }
    return true;
}

template<>
template<>
bool FileReader_CANoe::processingFunction<CANoe_Reader::Stat::READ_NEXT_LINE>(){
    QString line = readInfos.last()->file.readLine();
    line = line.trimmed();
    int stat = 0;
    (stat = (static_cast<int>(FSD_CANoe::FileSpecificInterpreterStat::BEGIN_OF_OBJECT) + 1) * static_cast<int>(line.contains("Begin_Of_Object"))) || (stat = (static_cast<int>(FSD_CANoe::FileSpecificInterpreterStat::END_OF_OBJECT) + 1) * static_cast<int>(line.contains("End_Of_Object"))) || (stat = (static_cast<int>(FSD_CANoe::FileSpecificInterpreterStat::DATA) + 1));
    if(stat != 0)
        stat--;
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
bool FileReader_CANoe::processingFunction<CANoe_Reader::Stat::CHECK_ERRORS>(){
    return !(!interpreter.resultChecking() || errorCode());
}

template<>
QVector<ProcessingFunctions_FR<CANoe_Reader>> FileReader<CANoe_Reader>::fileReader_processingFunctions = {
    &FileReader_CANoe::processingFunction<CANoe_Reader::Stat::CHECK_ERRORS>,
    &FileReader_CANoe::processingFunction<CANoe_Reader::Stat::CHECK_END_OF_FILE>,
    &FileReader_CANoe::processingFunction<CANoe_Reader::Stat::READ_NEXT_LINE>
};

//template<>
//constexpr FSD_CANoe::Reader::Error CompileTimeErrorControl<FSD_CANoe::Reader::Error>::compileTimeError(){
//    return FSD_CANoe::Reader::Error::NO_ERROR;
//}
