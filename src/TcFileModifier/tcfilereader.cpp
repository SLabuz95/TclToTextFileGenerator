#include"tcfilereader.hpp"
//#include"External/FileReader/Reader/filereaderprivreadinfo.hpp"
//#include"External/FileReader/Interpreter/filereaderinterpreterpriv.hpp"
using ByLine_Reader = FSD_ByLine::Reader;

template<>
QString FileReader_ByLine::fileFilters(){
    return QString();
}

template<>
bool FileReader_ByLine::processing(){
 /*
  * 1) End of File Check
  * 2) Next Line
  * 3) Check For Errors
  */
    readInfo.addStats({FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::PROCESSING), realReadStat(ByLine_Reader::Stat::READ_NEXT_LINE), realReadStat(ByLine_Reader::Stat::CHECK_END_OF_FILE)});
    return true;
}

template<>
bool FileReader_ByLine::initialize(){
    return FileReader_Priv::initialize();
}

template<>
bool FileReader_ByLine::deinitialize(){
    return FileReader_Priv::deinitialize();
}

template<>
template<>
bool FileReader_ByLine::processingFunction<ByLine_Reader::Stat::CHECK_END_OF_FILE>(){
    if(readInfo.getFile().atEnd()){    // Clear all stats and set CLOSE_FILE and DEINIT Stats
        readInfo.clearStats();
        readInfo.addStats({ FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::DEINITIALIZE), FileReader_Priv::realReadStat(FileReader_Priv_ReadStat::CLOSE_FILE),});
    }
    return true;
}

template<>
template<>
bool FileReader_ByLine::processingFunction<ByLine_Reader::Stat::READ_NEXT_LINE>(){
    QString line = readInfo.getFile().readLine();
    line = line.trimmed();
    int stat = static_cast<int>(FSD_ByLine::FileSpecificInterpreterStat::PROCESS_LINE);
    stat += static_cast<int>(FileReaderInterpreter_Priv_Stat::SIZE);
    NS_FRI_COM_Manager::ChannelDatas cmds = {
        {NS_FRI_COM_Manager::Commands::EXEC_STAT, &stat},
        {NS_FRI_COM_Manager::Commands::DATA, &line}
    };
    return processCOMMessage(cmds);
}


template<>
QVector<ProcessingFunctions_FR<ByLine_Reader>> FileReader<ByLine_Reader>::fileReader_processingFunctions = {
    &FileReader_ByLine::processingFunction<ByLine_Reader::Stat::CHECK_END_OF_FILE>,
    &FileReader_ByLine::processingFunction<ByLine_Reader::Stat::READ_NEXT_LINE>
};

//template<>
//constexpr FSD_ByLine::Reader::Error CompileTimeErrorControl<FSD_ByLine::Reader::Error>::compileTimeError(){
//    return FSD_ByLine::Reader::Error::NO_ERROR;
//}
